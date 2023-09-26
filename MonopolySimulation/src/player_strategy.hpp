#pragma once

#include <cassert>
#include <optional>
#include <tuple>
#include <utility>

#include "common_types.hpp"
#include "game_state.hpp"
#include "generic_sell_to_bank_iface.hpp"
#include "property_query.hpp"
#include "property_values.hpp"
#include "random.hpp"
#include "static_vector.hpp"


namespace monopoly {

	using sell_to_bank_choices_t = static_vector<generic_sell_to_bank_t, 8>;


	struct test_player_strategy_t {
		unsigned player;

		[[nodiscard]]
		bool should_buy_unowned_property(game_state_t const& game_state, random_t& random, street_t const street) {
			return _should_buy_unowned_property(game_state, random, street_values[street.global_index]);
		}

		[[nodiscard]]
		bool should_buy_unowned_property(game_state_t const& game_state, random_t& random, railway_t const railway) {
			return _should_buy_unowned_property(game_state, random, railway_value);
		}

		[[nodiscard]]
		bool should_buy_unowned_property(game_state_t const& game_state, random_t& random, utility_t const utility) {
			return _should_buy_unowned_property(game_state, random, utility_value);
		}

		[[nodiscard]]
		std::optional<card_type_t> should_use_get_out_of_jail_free(game_state_t const& game_state, random_t& random) {
			assert(game_state.get_out_of_jail_free_ownership.owns_any(player));
			// 50% chance of using the first card (and 50% chance of trying to roll doubles).
			if (random.uniform_bool()) {
				if (game_state.get_out_of_jail_free_ownership.is_owner(player, card_type_t::chance)) {
					return card_type_t::chance;
				}
				else {
					// This function shouldn't be called unless the player owns at least 1 card.
					assert(game_state.get_out_of_jail_free_ownership.is_owner(player, card_type_t::community_chest));
					return card_type_t::community_chest;
				}
			}
			return std::nullopt;
		}

		[[nodiscard]]
		sell_to_bank_choices_t choose_assets_for_forced_sale(game_state_t const& game_state, random_t& random,
				unsigned const min_amount) {
			// Sell in this order:
			//   - Streets with no buildings, cheapest first;
			//   - Utilities;
			//   - Railways;
			//   - Buildings, cheapest first.
			
			sell_to_bank_choices_t choices;

			long amount_remaining = min_amount;

			for (auto const& street : streets) {
				if (game_state.street_ownership.is_owner(player, street) && is_property_sellable(game_state, street)) {
					choices.push_back(generic_sell_to_bank_t{generic_sell_to_bank_type::street, street.global_index});
					amount_remaining -= property_sell_value(street);
					if (amount_remaining <= 0) {
						break;
					}
				}
			}

			if (amount_remaining > 0) {
				for (auto const utility : utilities) {
					if (game_state.utility_ownership.is_owner(player, utility)
							&& is_property_sellable(game_state, utility)) {
						choices.emplace_back(generic_sell_to_bank_type::utility, static_cast<unsigned>(utility));
						amount_remaining -= utility_sell_value;
						if (amount_remaining <= 0) {
							break;
						}
					}
				}
			}

			if (amount_remaining > 0) {
				for (auto const railway : railways) {
					if (game_state.railway_ownership.is_owner(player, railway)
							&& is_property_sellable(game_state, railway)) {
						choices.emplace_back(generic_sell_to_bank_type::railway, static_cast<unsigned>(railway));
						amount_remaining -= railway_sell_value;
						if (amount_remaining <= 0) {
							break;
						}
					}
				}
			}

			// TODO: buildings

			return choices;
		}

	private:
		[[nodiscard]]
		bool _should_buy_unowned_property(game_state_t const& game_state, random_t& random,
				unsigned const property_value) {
			// If the player has enough money, 50% chance of buying.
			auto const player_cash = game_state.players[player].cash;
			if (property_value > player_cash) {
				return false;
			}
			else {
				return random.uniform_bool();
			}
		}
	};


	struct player_strategies_t {
		std::tuple<
			test_player_strategy_t, test_player_strategy_t, test_player_strategy_t, test_player_strategy_t> strategies{
			{0}, {1}, {2}, {3}
		};

		player_strategies_t() = default;
		player_strategies_t& operator=(player_strategies_t&&) = default;

		template<typename F>
		decltype(auto) visit(unsigned const player, F&& func) {
			switch (player) {
			case 0:
				return std::forward<F>(func)(std::get<0>(strategies));
			case 1:
				return std::forward<F>(func)(std::get<1>(strategies));
			case 2:
				return std::forward<F>(func)(std::get<2>(strategies));
			case 3:
				return std::forward<F>(func)(std::get<3>(strategies));
			default:
				assert(false);
			}
			assert(false);
		}

	private:
		// Copying in other contexts is most likely a mistake.
		player_strategies_t(player_strategies_t const&) = default;
		player_strategies_t& operator=(player_strategies_t const&) = default;
	};

}
