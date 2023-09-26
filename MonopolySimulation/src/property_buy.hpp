#pragma once

#include <cassert>

#include "cash_basic.hpp"
#include "common_types.hpp"
#include "game_state.hpp"
#include "player_strategy.hpp"
#include "property_values.hpp"
#include "random.hpp"


namespace monopoly {

	// Gives ownership of an unowned property to the player, while paying the cost to the bank.
	// Assumes the player has enough cash on hand to make the purchase.
	template<PropertyType P>
	void buy_unowned_property(game_state_t& game_state, unsigned const player, P const property, unsigned const cost) {
		assert(!game_state.property_ownership<P>().is_owned(property));
		player_pay_bank_from_hand(game_state, player, cost);
		game_state.property_ownership<P>().set_owner(property, player);
	}


	// Gives the player the opportunity to buy an unowned property from the bank.
	// Return value indicates if the property was purchased or not.
	bool maybe_buy_unowned_property(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, PropertyType auto const property) {
		// TODO: allow the player to generate more cash to buy a property they otherwise can't afford.
		auto const cost = property_buy_cost(property);
		if (cost <= game_state.players[player].cash) {
			auto const buy = strategies.visit(player, [&game_state, &random, property](auto& strategy) {
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
