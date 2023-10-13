#pragma once

#include <cassert>

#include "game_state.hpp"
#include "generic_sell_to_bank.hpp"
#include "player_strategy.hpp"
#include "random.hpp"


namespace monopoly {

	// Sell off enough of a player's assets to generate the specified amount of cash.
	inline void force_sell_assets(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, unsigned const min_amount) {
		auto const& player_cash = game_state.players[player].cash;
		auto const cash_required = player_cash + min_amount;
		assert(min_amount > 0);
		while (true) {
			auto const sell_choices = strategies.visit(player,
				[&game_state, &random, min_amount](PlayerStrategy auto& strategy) {
					return strategy.choose_assets_for_forced_sale(game_state, random, min_amount);
				});
			
			if (sell_choices.empty()) {
				break;
			}

			for (auto const& sell : sell_choices) {
				generic_sell_to_bank(game_state, player, sell);
				if (player_cash >= cash_required) {
					break;
				}
			}
		}
	}

}
