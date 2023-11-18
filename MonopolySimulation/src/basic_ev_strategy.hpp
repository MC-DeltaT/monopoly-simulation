#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <ranges>
#include <utility>

#include "algorithm.hpp"
#include "board_space_constants.hpp"
#include "common_constants.hpp"
#include "common_types.hpp"
#include "game_state.hpp"
#include "gameplay_constants.hpp"
#include "math.hpp"
#include "property_constants.hpp"
#include "random.hpp"
#include "rent_constants.hpp"
#include "strategy_types.hpp"


// First attempt at an expected-value-based strategy.
// Basic calculations of immediate EV from various game events.
// Ignores a lot of stuff, e.g. longer term EV (like buying property), opportunity cost, complex game state effects.
namespace monopoly::basic_ev {

	template<typename T, size_t N>
	using distribution_t = std::array<std::pair<T, double>, N>;


	inline constexpr double avg_single_dice_roll = average(1, 2, 3, 4, 5, 6);

	// Distribution of the sum of 2 independent dice rolls.
	inline constexpr auto double_dice_roll_distribution = []{
		std::array<unsigned, 12 + 1> hist{};
		for (unsigned d1 = 1; d1 <= 6; ++d1) {
			for (unsigned d2 = 1; d2 <= 6; ++d2) {
				hist[d1 + d2]++;
			}
		}
		// Max is 6+6=12, min is 1+1=2
		distribution_t<unsigned, 12 - 2 + 1> dist{};
		for (unsigned i = 2; i <= 12; ++i) {
			dist[i - 2] = {i, static_cast<double>(hist[i]) / (6 * 6)};
		}
		return dist;
	}();

	// Distribution of the sum of 2 dice rolls where they are known to be the same dice value.
	inline constexpr distribution_t<unsigned, 6> doubles_roll_distribution{{
		{2, 1.0/6}, {4, 1.0/6}, {6, 1.0/6}, {8, 1.0/6}, {10, 1.0/6}, {12, 1.0/6}
	}};

	// Distribution of the sum of 2 dice rolls where they are known to be different dice values.
	inline constexpr auto not_doubles_roll_distribution = []{
		std::array<unsigned, 12 + 1> hist{};
		for (unsigned d1 = 1; d1 <= 6; ++d1) {
			for (unsigned d2 = 1; d2 <= 6; ++d2) {
				if (d1 != d2) {
					hist[d1 + d2]++;
				}
			}
		}
		// Max is 5+6=11, min is 1+2=3
		distribution_t<unsigned, 11 - 3 + 1> dist{};
		for (unsigned i = 3; i <= 11; ++i) {
			dist[i - 3] = {i, hist[i] / 30.0};
		}
		return dist;
	}();


	// Probability on each turn that you have to pay the jail fine to be released, if rolling doubles.
	inline constexpr auto jail_fine_chance =
		std::views::iota(0u, max_turns_in_jail) | std::views::reverse | std::views::transform([](unsigned const i) {
			return cpow(5.0 / 6.0, i + 1);
		});

	// For each turn in jail.
	inline constexpr auto get_out_of_jail_free_value = jail_fine_chance | std::views::transform([](double const p) {
		return p * jail_release_cost;
	});


	inline constexpr double go_space_ev = go_salary;

	inline constexpr double income_tax_space_ev = -static_cast<double>(income_tax);

	inline constexpr double super_tax_space_ev = -static_cast<double>(super_tax);


	inline constexpr double go_to_jail_ev = -jail_fine_chance[0] * jail_release_cost;

	inline constexpr double just_visiting_jail_space_ev = 0;

	inline constexpr double free_parking_space_ev = 0;


	struct lookahead_state_t {
		unsigned movement_roll = 0;
		unsigned railway_rent_multiplier = 1;		// Applied by the "advance to next railway" card.
		unsigned utility_rent_dice_multiplier_override = 0;		// Applied by the "advance to next utility" card.
	};


	// TODO: can do better than this
	inline constexpr double chance_card_ev_const = average<chance_card_count>({
		go_salary,		// Advance to Go, ignoring any board-position-related-EV.
		0, 0, 0, 0, 0, 0, 0, 0,		// Various movement cards, ignore board-position-related-EV.
		go_to_jail_ev,	// Go to jail
		get_out_of_jail_free_value[0],	// Get Out Of Jail Free card, assuming it's unowned.
		50, 150,	// Fixed cash awards.
		-15,		// Fixed cash fees.
		-50.0 * player_count,	// Per-player cash fee, assuming no one is bankrupt.
		0		// Per-building cash fee - buildings not implemented yet.
	});

	// TODO: can do better than this
	inline constexpr double community_chest_card_ev_const = average<community_chest_card_count>({
		go_salary,		// Advance to Go, ignoring any board-position-related-EV.
		go_to_jail_ev,	// Go to jail
		get_out_of_jail_free_value[0],	// Get Out Of Jail Free card, assuming it's unowned.
		10, 20, 25, 50, 100, 100, 100, 200,	// Fixed cash awards.
		10 * player_count,	// Per player cash award, assuming no one is bankrupt.
		-50, -50, -100,		// Fixed cash fees.
		0		// Per-building cash fee - buildings not implemented yet.
	});


	[[nodiscard]]
	inline double chance_card_ev(game_state_t const& game_state, unsigned const player) {
		// TODO: look at game state for better EV.
		return chance_card_ev_const;
	}

	[[nodiscard]]
	inline double community_chest_card_ev(game_state_t const& game_state, unsigned const player) {
		// TODO: look at game state for better EV.
		return community_chest_card_ev_const;
	}


	[[nodiscard]]
	inline double street_space_ev(game_state_t const& game_state, unsigned const player, street_t const street) {
		auto const owner = game_state.property_ownership.street.get_owner(street);
		if (!owner.has_value()) {
			return 0;
		}
		else if (*owner == player) {
			return 0;
		}
		else if (game_state.street_development.is_mortgaged(street)) {
			return 0;
		}
		else {
			auto const building_level = game_state.street_development.building_level(street);
			unsigned rent = street_rents[street.generic_index][building_level];
			auto const owns_entire_set =
				game_state.property_ownership.street.owns_entire_colour_set(*owner, street.colour_set);
			if (building_level == 0 && owns_entire_set) {
				// No houses or hotel present, and all streets in the colour set are owned.
				rent *= full_colour_set_rent_multiplier;
			}
			return -static_cast<double>(rent);
		}
	}

	[[nodiscard]]
	inline double railway_space_ev(game_state_t const& game_state, unsigned const player, railway_t const railway,
			lookahead_state_t const& lookahead) {
		auto const owner = game_state.property_ownership.railway.get_owner(railway);
		if (!owner.has_value()) {
			return 0;
		}
		else if (*owner == player) {
			return 0;
		}
		else if (game_state.railway_development.is_mortgaged(railway)) {
			return 0;
		}
		else {
			auto const railways_owned = game_state.property_ownership.railway.owned_count(*owner);
			assert(railways_owned >= 1u);
			auto const rent = railway_rents[railways_owned - 1u] * lookahead.railway_rent_multiplier;
			return -static_cast<double>(rent);
		}
	}

	[[nodiscard]]
	inline double utility_space_ev(game_state_t const& game_state, unsigned const player, utility_t const utility,
			lookahead_state_t const& lookahead) {
		auto const owner = game_state.property_ownership.utility.get_owner(utility);
		if (!owner.has_value()) {
			return 0;
		}
		else if (*owner == player) {
			return 0;
		}
		else if (game_state.utility_development.is_mortgaged(utility)) {
			return 0;
		}
		else {
			if (lookahead.utility_rent_dice_multiplier_override == 0) {
				// Normal turn landing on a utility.
				auto const utilities_owned = game_state.property_ownership.utility.owned_count(*owner);
				assert(utilities_owned >= 1u);
				auto const rent = lookahead.movement_roll * utility_rent_dice_multiplier[utilities_owned - 1u];
				return rent;
			}
			else {
				// Sent to a utility by a card.
				auto const rent = avg_single_dice_roll * lookahead.utility_rent_dice_multiplier_override;
				return -static_cast<double>(rent);
			}
		}
	}


	[[nodiscard]]
	inline double board_space_ev(game_state_t const& game_state, unsigned const player,
			board_space_t const board_space, lookahead_state_t& lookahead) {
		// TODO: handle cards better
		switch (board_space) {
		case board_space_t::go:
			return go_space_ev;
		case board_space_t::old_kent_road:
			return street_space_ev(game_state, player, std::get<0>(streets));
		case board_space_t::community_chest_1:
			return community_chest_card_ev(game_state, player);
		case board_space_t::whitechapel_road:
			return street_space_ev(game_state, player, std::get<1>(streets));
		case board_space_t::income_tax:
			return income_tax_space_ev;
		case board_space_t::kings_cross_station:
			return railway_space_ev(game_state, player, railway_t::kings_cross, lookahead);
		case board_space_t::the_angel_islington:
			return street_space_ev(game_state, player, std::get<2>(streets));
		case board_space_t::chance_1:
			return chance_card_ev(game_state, player);
		case board_space_t::euston_road:
			return street_space_ev(game_state, player, std::get<3>(streets));
		case board_space_t::pentonville_road:
			return street_space_ev(game_state, player, std::get<4>(streets));
		case board_space_t::just_visiting_jail:
			return just_visiting_jail_space_ev;
		case board_space_t::pall_mall:
			return street_space_ev(game_state, player, std::get<5>(streets));
		case board_space_t::electric_company:
			return utility_space_ev(game_state, player, utility_t::electric_company, lookahead);
		case board_space_t::whitehall:
			return street_space_ev(game_state, player, std::get<6>(streets));
		case board_space_t::northumberland_avenue:
			return street_space_ev(game_state, player, std::get<7>(streets));
		case board_space_t::marylebone_station:
			return railway_space_ev(game_state, player, railway_t::marylebone, lookahead);
		case board_space_t::bow_street:
			return street_space_ev(game_state, player, std::get<8>(streets));
		case board_space_t::community_chest_2:
			return community_chest_card_ev(game_state, player);
		case board_space_t::marlborough_street:
			return street_space_ev(game_state, player, std::get<9>(streets));
		case board_space_t::vine_street:
			return street_space_ev(game_state, player, std::get<10>(streets));
		case board_space_t::free_parking:
			return free_parking_space_ev;
		case board_space_t::strand:
			return street_space_ev(game_state, player, std::get<11>(streets));
		case board_space_t::chance_2:
			return chance_card_ev(game_state, player);
		case board_space_t::fleet_street:
			return street_space_ev(game_state, player, std::get<12>(streets));
		case board_space_t::trafalgar_square:
			return street_space_ev(game_state, player, std::get<13>(streets));
		case board_space_t::fenchurch_street_station:
			return railway_space_ev(game_state, player, railway_t::fenchurch_street, lookahead);
		case board_space_t::leicester_square:
			return street_space_ev(game_state, player, std::get<14>(streets));
		case board_space_t::coventry_street:
			return street_space_ev(game_state, player, std::get<15>(streets));
		case board_space_t::water_works:
			return utility_space_ev(game_state, player, utility_t::water_works, lookahead);
		case board_space_t::piccadilly:
			return street_space_ev(game_state, player, std::get<16>(streets));
		case board_space_t::go_to_jail:
			return go_to_jail_ev;
		case board_space_t::regent_street:
			return street_space_ev(game_state, player, std::get<17>(streets));
		case board_space_t::oxford_street:
			return street_space_ev(game_state, player, std::get<18>(streets));
		case board_space_t::community_chest_3:
			return community_chest_card_ev(game_state, player);
		case board_space_t::bond_street:
			return street_space_ev(game_state, player, std::get<19>(streets));
		case board_space_t::liverpool_street_station:
			return railway_space_ev(game_state, player, railway_t::liverpool_street, lookahead);
		case board_space_t::chance_3:
			return chance_card_ev(game_state, player);
		case board_space_t::park_lane:
			return street_space_ev(game_state, player, std::get<20>(streets));
		case board_space_t::super_tax:
			return super_tax_space_ev;
		case board_space_t::mayfair:
			return street_space_ev(game_state, player, std::get<21>(streets));
		default:
			assert(false);
			break;
		}
	}


	// TODO: want to take into account doubles roll which gives another turn

	[[nodiscard]]
	inline double movement_roll_ev(game_state_t const& game_state, unsigned const player, unsigned const roll) {
		auto const& player_state = game_state.players[player];
		assert(!player_state.is_bankrupt());
		assert(roll > 0 && roll <= 12);

		double ev = 0;

		auto const position =
			player_state.in_jail() ? static_cast<unsigned>(board_space_t::just_visiting_jail) : player_state.position;

		auto new_position = position + roll;
		if (new_position >= board_space_count) {
			// If landing exactly on Go, salary will be accounted for in the board space handling.
			if (new_position > board_space_count) {
				ev += go_salary;
			}
			new_position -= board_space_count;
		}
		
		auto const new_space = static_cast<board_space_t>(new_position);

		lookahead_state_t lookahead;
		lookahead.movement_roll = roll;
		ev += board_space_ev(game_state, player, new_space, lookahead);

		return ev;
	}

	template<std::size_t N>
	[[nodiscard]]
	double movement_ev(game_state_t const& game_state, unsigned const player,
			distribution_t<unsigned, N> const& roll_dist) {
		double ev = 0;
		for (auto const [roll, probability] : roll_dist) {
			ev += probability * movement_roll_ev(game_state, player, roll);
		}
		return ev;
	}


	[[nodiscard]]
	inline std::pair<in_jail_action_t, double> decide_jail_action_impl(game_state_t const& game_state,
			unsigned const player, unsigned const turn_in_jail) {
		assert(turn_in_jail < max_turns_in_jail);

		auto const normal_roll_ev = movement_ev(game_state, player, double_dice_roll_distribution);
		auto const doubles_roll_ev = movement_ev(game_state, player, doubles_roll_distribution);
		auto const not_doubles_roll_ev = movement_ev(game_state, player, not_doubles_roll_distribution);

		auto const pay_fine_ev = -static_cast<double>(jail_release_cost) + normal_roll_ev;
		auto const use_card_ev = -get_out_of_jail_free_value[turn_in_jail] + normal_roll_ev;
		auto const next_turn_ev = turn_in_jail + 1 < max_turns_in_jail
			? decide_jail_action_impl(game_state, player, turn_in_jail + 1).second
			// If we failed to roll doubles on the last turn, must pay fine and move. Know dice roll is not a double.
			: -static_cast<double>(jail_release_cost) + not_doubles_roll_ev;
		auto const roll_doubles_ev = 1.0 / 6.0 * doubles_roll_ev + 5.0 / 6.0 * next_turn_ev;

		auto const use_card_best = use_card_ev >= roll_doubles_ev && use_card_ev >= pay_fine_ev;
		if (use_card_best && game_state.get_out_of_jail_free_ownership.is_owner(player, card_type_t::chance)) {
			return {in_jail_action_t::get_out_of_jail_free_chance, use_card_ev};
		}
		else if (use_card_best
				&& game_state.get_out_of_jail_free_ownership.is_owner(player, card_type_t::community_chest)) {
			return {in_jail_action_t::get_out_of_jail_free_community_chest, use_card_ev};
		}
		if (roll_doubles_ev >= pay_fine_ev) {
			assert(roll_doubles_ev >= use_card_ev || !game_state.get_out_of_jail_free_ownership.owns_any(player));
			return {in_jail_action_t::roll_doubles, roll_doubles_ev};
		}
		else {
			assert(pay_fine_ev >= roll_doubles_ev && pay_fine_ev >= use_card_ev);
			return {in_jail_action_t::pay_fine, pay_fine_ev};
		}
	}

	struct jail_strategy_t {
		[[nodiscard]]
		static in_jail_action_t decide_jail_action(game_state_t const& game_state, random_t&, unsigned const player) {
			auto const& player_state = game_state.players[player];
			assert(player_state.in_jail());
			auto const turn_in_jail = player_state.position + static_cast<int>(max_turns_in_jail);
			assert(turn_in_jail >= 0);
			return decide_jail_action_impl(game_state, player, static_cast<unsigned>(turn_in_jail)).first;
		}
	};

}
