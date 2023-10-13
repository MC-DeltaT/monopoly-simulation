#pragma once

#include <cassert>
#include <utility>

#include "cash_basic.hpp"
#include "common_types.hpp"
#include "game_state.hpp"
#include "player_strategy.hpp"
#include "property_values.hpp"
#include "random.hpp"
#include "statistics_counters.hpp"


namespace monopoly {

	// Gives ownership of an unowned property to the player, while paying the cost to the bank.
	// Assumes the player has enough cash on hand to make the purchase.
	template<PropertyType P>
	void buy_unowned_property(game_state_t& game_state, unsigned const player, P const property, unsigned const cost) {
		assert(!game_state.property_ownership.get<P>().is_owned(property));
		player_pay_bank_from_hand(game_state, player, cost);
		game_state.property_ownership.get<P>().set_owner(property, player);

		auto const property_idx = static_cast<unsigned>(property);
		if (!std::exchange(stat_helper_state.property_has_been_purchased.get<P>()[property_idx], true)) {
			stat_counters.property_purchased_at_least_once.get<P>()[property_idx]++;
			stat_counters.property_first_purchase_round.get<P>()[property_idx] += game_state.round + 1;
		}
	}


	// Gives the player the opportunity to buy an unowned property from the bank.
	// Return value indicates if the property was purchased or not.
	bool maybe_buy_unowned_property(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, PropertyType auto const property) {
		// TODO: allow the player to generate more cash to buy a property they otherwise can't afford.
		auto const cost = property_buy_cost(property);
		if (cost <= game_state.players[player].cash) {
			auto const buy = strategies.visit(player, [&game_state, &random, property](PlayerStrategy auto& strategy) {
				return strategy.should_buy_unowned_property(game_state, random, property);
			});
			if (buy) {
				buy_unowned_property(game_state, player, property, cost);
				return true;
			}
		}
		return false;
	}

}
