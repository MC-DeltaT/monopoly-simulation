#pragma once

#include <cassert>
#include <optional>

#include "cash_basic.hpp"
#include "common_types.hpp"
#include "game_state.hpp"
#include "property_query.hpp"
#include "property_values.hpp"
#include "statistics_counters.hpp"


namespace monopoly {

	template<PropertyType P>
	void sell_property_to_bank(game_state_t& game_state, unsigned const player, P const property) {
		assert(game_state.property_ownership.get<P>().is_owner(player, property));
		assert(is_property_sellable(game_state, property));

		game_state.property_ownership.get<P>().set_owner(property, std::nullopt);
		auto const sell_amount = property_sell_value(property);
		bank_pay_player(game_state, player, sell_amount);

		if constexpr (record_stats) {
			stat_counters.property_sell_income[player] += sell_amount;
		}
	}


	// TODO: selling buildings

}
