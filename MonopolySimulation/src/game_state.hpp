#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <numeric>
#include <optional>
#include <utility>

#include "algorithm.hpp"
#include "card_constants.hpp"
#include "common_constants.hpp"
#include "common_types.hpp"
#include "gameplay_constants.hpp"
#include "per_propertytype_data.hpp"
#include "property_constants.hpp"
#include "safe_numeric.hpp"


namespace monopoly {

	class street_ownership_t {
	public:
		constexpr street_ownership_t() noexcept {
			for (auto& owners : _colour_set_owners) {
				owners.fill(-1);
			}
		}

		[[nodiscard]]
		constexpr std::optional<unsigned> get_owner(street_t const street) const {
			auto const owner = _colour_set_owners[street.colour_set][street.index_in_set];
			if (owner >= 0) {
				return owner;
			}
			else {
				return std::nullopt;
			}
		}

		constexpr void set_owner(street_t const street, std::optional<unsigned> const new_owner) {
			auto& owner = _colour_set_owners[street.colour_set][street.index_in_set];
			if (new_owner.has_value()) {
				safe_int_assign(owner, *new_owner);
			}
			else {
				owner = -1;
			}
		}

		[[nodiscard]]
		constexpr bool is_owned(street_t const street) const {
			return get_owner(street).has_value();
		}

		[[nodiscard]]
		constexpr bool is_owner(unsigned const player, street_t const street) const {
			return get_owner(street) == player;
		}

		[[nodiscard]]
		constexpr unsigned owned_count_in_colour_set(unsigned const player, unsigned const colour_set) const {
			unsigned count = 0;
			for (auto const owner : _colour_set_owners[colour_set]) {
				count += std::cmp_equal(owner, player);
			}
			return count;
		}

		[[nodiscard]]
		constexpr bool owns_entire_colour_set(unsigned const player, unsigned const colour_set) const {
			auto const set_size = colour_set_sizes[colour_set];
			auto const owned_in_set = owned_count_in_colour_set(player, colour_set);
			return owned_in_set == set_size;
		}

	private:
		// Negative = unowned, Nonnegative = owning player.
		std::array<std::array<int, max_colour_set_size>, colour_set_count> _colour_set_owners;
	};


	class street_development_t {
	public:
		[[nodiscard]]
		constexpr bool is_mortgaged(street_t const street) const {
			return _colour_set_state[street.colour_set][street.index_in_set] < 0;
		}

		constexpr void set_mortaged(street_t const street) {
			// Can't mortgage a property with buildings.
			assert(building_level(street) == 0);
			_colour_set_state[street.colour_set][street.index_in_set] = -1;
		}

		constexpr void set_unmortgaged(street_t const street) {
			assert(is_mortgaged(street));
			_colour_set_state[street.colour_set][street.index_in_set] = 0;
		}

		constexpr void remove_building(street_t const street) {
			assert(building_level(street) >= 1);
			--_colour_set_state[street.colour_set][street.index_in_set];
		}

		constexpr void remove_all_buildings(street_t const street) {
			_colour_set_state[street.colour_set][street.index_in_set] = 0;
		}

		[[nodiscard]]
		constexpr unsigned house_count(street_t const street) const {
			auto const state = _colour_set_state[street.colour_set][street.index_in_set];
			if (state >= 1 && state <= 4) {
				return state;
			}
			else {
				return 0;
			}
		}

		[[nodiscard]]
		constexpr unsigned hotel_count(street_t const street) const {
			auto const state = _colour_set_state[street.colour_set][street.index_in_set];
			assert(state <= 5);		// Can have at most 1 hotel.
			return state == 5;
		}

		// 0 = no buildings, 1-4 = houses, 5 = hotel.
		[[nodiscard]]
		constexpr unsigned building_level(street_t const street) const {
			auto const state = _colour_set_state[street.colour_set][street.index_in_set];
			if (state < 0) {
				return 0;
			}
			else {
				assert(state <= 5);		// Can have at most 1 hotel.
				return state;
			}
		}

		// -1 = mortgaged, 0 = no houses, 1-4 = houses, 5 = hotel.
		[[nodiscard]]
		constexpr int development_level(street_t const street) const {
			auto const level = _colour_set_state[street.colour_set][street.index_in_set];
			assert(level >= -1 && level <= 5);
			return level;
		}

		[[nodiscard]]
		constexpr bool colour_set_has_buildings(unsigned const colour_set) const {
			return std::ranges::any_of(_colour_set_state[colour_set], [](auto const state) {
				return state > 0;
			});
		}

		[[nodiscard]]
		constexpr int min_development_level_in_set(unsigned const colour_set) const {
			auto const level = std::ranges::min(_colour_set_state[colour_set]);
			assert(level >= -1 && level <= 5);
			return level;
		}

		[[nodiscard]]
		constexpr int max_development_level_in_set(unsigned const colour_set) const {
			auto const level = std::ranges::max(_colour_set_state[colour_set]);
			assert(level >= -1 && level <= 5);
			return level;
		}

	private:
		// -1 = mortgaged, 0 = no houses, 1-4 = houses, 5 = hotel.
		std::array<std::array<int, max_colour_set_size>, colour_set_count> _colour_set_state{};
	};


	class railway_ownership_t {
	public:
		constexpr railway_ownership_t() noexcept {
			_owners.fill(-1);
		}

		[[nodiscard]]
		constexpr std::optional<unsigned> get_owner(railway_t const railway) const {
			auto const owner = _owners[static_cast<unsigned>(railway)];
			if (owner >= 0) {
				return owner;
			}
			else {
				return std::nullopt;
			}
		}

		constexpr void set_owner(railway_t const railway, std::optional<unsigned> const new_owner) {
			auto& owner = _owners[static_cast<unsigned>(railway)];
			if (new_owner.has_value()) {
				safe_int_assign(owner, *new_owner);
			}
			else {
				owner = -1;
			}
		}

		[[nodiscard]]
		constexpr bool is_owned(railway_t const railway) const {
			return get_owner(railway).has_value();
		}

		[[nodiscard]]
		constexpr bool is_owner(unsigned const player, railway_t const railway) const {
			return get_owner(railway) == player;
		}

		[[nodiscard]]
		constexpr unsigned owned_count(unsigned const player) const {
			unsigned count = 0;
			for (auto const owner : _owners) {
				count += std::cmp_equal(owner, player);
			}
			return count;
		}

	private:
		// Negative = unowned, Nonnegative = owning player.
		std::array<int, railway_count> _owners;
	};


	class railway_development_t {
	public:
		[[nodiscard]]
		constexpr bool is_mortgaged(railway_t const railway) const {
			return _mortgaged[static_cast<unsigned>(railway)];
		}

		constexpr void set_mortgaged(railway_t const railway) {
			_mortgaged[static_cast<unsigned>(railway)] = true;
		}

		constexpr void set_unmortgaged(railway_t const railway) {
			_mortgaged[static_cast<unsigned>(railway)] = false;
		}

	private:
		std::array<bool, railway_count> _mortgaged{};
	};


	class utility_ownership_t {
	public:
		constexpr utility_ownership_t() noexcept {
			_owners.fill(-1);
		}
		
		[[nodiscard]]
		constexpr std::optional<unsigned> get_owner(utility_t const utility) const {
			auto const owner = _owners[static_cast<unsigned>(utility)];
			if (owner >= 0) {
				return owner;
			}
			else {
				return std::nullopt;
			}
		}

		constexpr void set_owner(utility_t const utility, std::optional<unsigned> const new_owner) {
			auto& owner = _owners[static_cast<unsigned>(utility)];
			if (new_owner.has_value()) {
				safe_int_assign(owner, *new_owner);
			}
			else {
				owner = -1;
			}
		}

		[[nodiscard]]
		constexpr bool is_owned(utility_t const utility) const {
			return get_owner(utility).has_value();
		}

		[[nodiscard]]
		constexpr bool is_owner(unsigned const player, utility_t const utility) const {
			return get_owner(utility) == player;
		}

		[[nodiscard]]
		constexpr unsigned owned_count(unsigned const player) const {
			unsigned count = 0;
			for (auto const owner : _owners) {
				count += std::cmp_equal(owner, player);
			}
			return count;
		}

	private:
		// Negative = unowned, Nonnegative = owning player.
		std::array<int, utility_count> _owners;
	};


	class utility_development_t {
	public:
		[[nodiscard]]
		constexpr bool is_mortgaged(utility_t const utility) const {
			return _mortgaged[static_cast<unsigned>(utility)];
		}

		constexpr void set_mortgaged(utility_t const utility) {
			_mortgaged[static_cast<unsigned>(utility)] = true;
		}

		constexpr void set_unmortgaged(utility_t const utility) {
			_mortgaged[static_cast<unsigned>(utility)] = false;
		}

	private:
		std::array<bool, utility_count> _mortgaged{};
	};


	// TODO: maybe get rid of this class, move card logic into free functions
	template<typename CardType, CardType GetOutOfJailFree, unsigned Size>
	class card_deck_t {
	public:
		static_assert(Size > 0);
		static_assert(static_cast<unsigned long long>(GetOutOfJailFree) < Size);

		constexpr card_deck_t() noexcept {
			for (unsigned i = 0; i < Size; ++i) {
				_cards[i] = CardType{i};
			}
		}

		[[nodiscard]]
		constexpr CardType draw_card(bool const is_get_out_of_jail_free_owned) noexcept {
			assert(is_get_out_of_jail_free_owned == _get_out_of_jail_free_taken);

			auto card = _cards[_top_index];
			if (card == GetOutOfJailFree) {
				if (is_get_out_of_jail_free_owned) {
					// If the card is Get Out Of Jail Free and it's already taken, try again.
					_inc_top();
					card = _cards[_top_index];
					assert(card != GetOutOfJailFree);
				}
				else {
					_get_out_of_jail_free_index = _top_index;
#ifndef NDEBUG
					_get_out_of_jail_free_taken = true;
#endif
				}
			}
			_inc_top();
			return card;
		}

		// Places the Get Out Of Jail Free card at the back of the deck.
		// The card must have been drawn from the deck previously.
		constexpr void return_get_out_of_jail_free() {
			assert(_get_out_of_jail_free_taken);

			// TODO

#ifndef NDEBUG
			_get_out_of_jail_free_taken = false;
#endif
		}

		void shuffle(auto& random_engine) {
			fast_shuffle(_cards, random_engine);
		}

	private:
		std::array<CardType, Size> _cards;
		unsigned _top_index = 0;	// Next card to draw.
		unsigned _get_out_of_jail_free_index = 0;	// Only valid when Get Out Of Jail Free card has been drawn.
#ifndef NDEBUG
		// Don't want to track this in an optimised build because that's redundant state!
		bool _get_out_of_jail_free_taken = false;
#endif

		static_assert(std::cmp_less_equal(Size - 1, std::numeric_limits<decltype(_top_index)>::max()));

		constexpr void _inc_top() noexcept {
			_top_index = (_top_index + 1u) % Size;
		}
	};


	class get_out_of_jail_free_card_ownership_t {
	public:
		constexpr get_out_of_jail_free_card_ownership_t() noexcept {
			_owners.fill(-1);
		}

		[[nodiscard]]
		constexpr std::optional<unsigned> get_owner(card_type_t const card) const {
			auto const owner = _owners[static_cast<unsigned>(card)];
			if (owner >= 0) {
				return owner;
			}
			else {
				return std::nullopt;
			}
		}

		[[nodiscard]]
		constexpr bool is_owner(unsigned const player, card_type_t const card) const {
			return get_owner(card) == player;
		}

		[[nodiscard]]
		constexpr bool is_owned(card_type_t const card) const {
			return get_owner(card).has_value();
		}

		constexpr void set_owner(card_type_t const card, std::optional<unsigned> const new_owner) {
			auto& owner = _owners[static_cast<unsigned>(card)];
			if (new_owner.has_value()) {
				safe_int_assign(owner, *new_owner);
			}
			else {
				owner = -1;
			}
		}

		[[nodiscard]]
		constexpr bool owns_any(unsigned const player) const noexcept {
			for (auto const owner : _owners) {
				if (std::cmp_equal(owner, player)) {
					return true;
				}
			}
			return false;
		}

	private:
		// Negative = no owner, nonnegative = owning player.
		std::array<int, 2> _owners;
	};


	struct player_state_t {
		int position = 0;		// Nonegative = board index, negative = in jail.
		std::optional<unsigned> bankrupt_round;		// Round at which the player became bankrupt.
		unsigned consecutive_doubles = 0;
		std::uint32_t cash = initial_cash;
		unsigned houses_owned = 0;
		unsigned hotels_owned = 0;

		[[nodiscard]]
		constexpr board_space_t get_board_space() const {
			// In jail doesn't correspond to any board space, it's handled separately.
			assert(position >= 0);
			return static_cast<board_space_t>(position);
		}

		[[nodiscard]]
		constexpr bool in_jail() const noexcept {
			return position < 0;
		}

		[[nodiscard]]
		constexpr bool is_bankrupt() const noexcept {
			return bankrupt_round.has_value();
		}
	};


	struct turn_state_t {
		unsigned movement_roll = 0;
		unsigned railway_rent_multiplier = 1;		// Applied by the "advance to next railway" card.
		unsigned utility_rent_dice_multiplier_override = 0;		// Applied by the "advance to next utility" card.
		bool position_changed = false;
	};


	struct game_state_t {
		std::array<player_state_t, player_count> players;
		per_propertytype_data<street_ownership_t, railway_ownership_t, utility_ownership_t> property_ownership;
		per_propertytype_data<street_development_t, railway_development_t, utility_development_t> property_development;
		street_development_t street_development;
		railway_development_t railway_development;
		utility_development_t utility_development;
		// unsigned houses_available = total_houses;
		// unsigned hotels_available = total_hotels;
		card_deck_t<chance_card_t, chance_card_t::get_out_of_jail_free, chance_card_count> chance_deck;
		card_deck_t<community_chest_card_t, community_chest_card_t::get_out_of_jail_free, community_chest_card_count>
			community_chest_deck;
		get_out_of_jail_free_card_ownership_t get_out_of_jail_free_ownership;
		unsigned round = 0;
		turn_state_t turn;

		game_state_t() = default;
		game_state_t& operator=(game_state_t&&) = default;

		template<card_type_t C>
		[[nodiscard]]
		constexpr auto& card_deck() noexcept {
			if constexpr (C == card_type_t::chance) {
				return chance_deck;
			}
			else if constexpr (C == card_type_t::community_chest) {
				return community_chest_deck;
			}
		}

	private:
		// Copying in other contexts is most likely a mistake.
		game_state_t(game_state_t const&) = default;
		game_state_t& operator=(game_state_t const&) = default;
	};


	struct auction_state_t {
		// 0 represents "no bid", since cannot buy a property for $0.
		std::array<unsigned, player_count> bids{};
	};

}
