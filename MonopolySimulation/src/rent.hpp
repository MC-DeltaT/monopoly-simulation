#pragma once

#include <cassert>

#include "cash.hpp"
#include "common_types.hpp"
#include "game_state.hpp"
#include "player_strategy.hpp"
#include "random.hpp"
#include "rent_constants.hpp"
#include "statistics_counters.hpp"


namespace monopoly {

	// Calculates the rent payable on a street. Assumes the street is owned.
	[[nodiscard]]
	inline unsigned calculate_rent(game_state_t const& game_state, street_t const street) {
		auto const owner = game_state.property_ownership.street.get_owner(street);
		assert(owner.has_value());
		if (game_state.street_development.is_mortgaged(street)) {
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
			return rent;
		}
	}

	// Calculates the rent payable on a railway.
	// Assumes the railway is owned and the payer is player whose turn it is.
	[[nodiscard]]
	inline unsigned calculate_rent(game_state_t const& game_state, railway_t const railway) {
		auto const owner = game_state.property_ownership.railway.get_owner(railway);
		assert(owner.has_value());
		if (game_state.railway_development.is_mortgaged(railway)) {
			return 0;
		}
		else {
			auto const railways_owned = game_state.property_ownership.railway.owned_count(*owner);
			assert(railways_owned >= 1u);
			auto const rent = railway_rents[railways_owned - 1u] * game_state.turn.railway_rent_multiplier;
			return rent;
		}
	}

	// Calculates the rent payable on a utility.
	// Assumes the utility is owned and the payer is the player whose turn it is.
	[[nodiscard]]
	inline unsigned calculate_rent(game_state_t const& game_state, random_t& random, utility_t const utility) {
		auto const owner = game_state.property_ownership.utility.get_owner(utility);
		assert(owner.has_value());
		if (game_state.utility_development.is_mortgaged(utility)) {
			return 0;
		}
		else {
			if (game_state.turn.utility_rent_dice_multiplier_override == 0) {
				// Normal turn landing on a utility.
				auto const utilities_owned = game_state.property_ownership.utility.owned_count(*owner);
				assert(utilities_owned >= 1u);
				auto const rent = game_state.turn.movement_roll * utility_rent_dice_multiplier[utilities_owned - 1u];
				return rent;
			}
			else {
				// Sent to a utility by a card.
				auto const rent = random.single_dice_roll() * game_state.turn.utility_rent_dice_multiplier_override;
				return rent;
			}
		}
	}


	// Pays the owner of the property the applicable rent, if the player is not the owner.
	// Assumes the property is owned.
	template<PropertyType P>
	void pay_rent(game_state_t& game_state, player_strategies_t& strategies, random_t& random, unsigned const player,
			P const property) {
		auto const owner = game_state.property_ownership.get<P>().get_owner(property);
		assert(owner.has_value());
		if (*owner != player) {
			unsigned rent;
			if constexpr (std::same_as<P, utility_t>) {
				rent = calculate_rent(game_state, random, property);
			}
			else {
				rent = calculate_rent(game_state, property);
			}
			player_pay_player(game_state, strategies, random, player, *owner, rent);

			if constexpr (record_stats) {
				stat_counters.rent_paid_amount[player] += rent;
				stat_counters.rent_received_amount[*owner] += rent;
				stat_counters.rent_paid_count[player]++;
				stat_counters.rent_received_count[*owner]++;
			}
		}
	}

}
