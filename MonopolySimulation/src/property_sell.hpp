#pragma once

#include <cassert>
#include <optional>

#include "cash_basic.hpp"
#include "common_types.hpp"
#include "game_state.hpp"
#include "property_query.hpp"
#include "property_values.hpp"


namespace monopoly {

	template<PropertyType P>
	void sell_property_to_bank(game_state_t& game_state, unsigned const player, P const property) {
		assert(game_state.property_ownership<P>().is_owner(player, property));
		assert(is_property_sellable(game_state, property));

		game_state.property_ownership<P>().set_owner(property, std::nullopt);
		bank_pay_player(game_state, player, property_sell_value(property));
	}


	// TODO: selling buildings

}
