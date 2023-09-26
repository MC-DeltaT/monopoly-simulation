#pragma once

#include <cassert>

#include "common_types.hpp"
#include "game_state.hpp"
#include "generic_sell_to_bank_iface.hpp"
#include "property_constants.hpp"
#include "property_sell.hpp"


namespace monopoly {

	inline void generic_sell_to_bank(game_state_t& game_state, unsigned const player,
			generic_sell_to_bank_t const& sell) {
		switch (sell.type) {
		case generic_sell_to_bank_type::street: {
			auto const& street = streets[sell.data1];
			sell_property_to_bank(game_state, player, street);
			break;
		}
		case generic_sell_to_bank_type::railway: {
			auto const railway = static_cast<railway_t>(sell.data1);
			sell_property_to_bank(game_state, player, railway);
			break;
		}
		case generic_sell_to_bank_type::utility: {
			auto const utility = static_cast<utility_t>(sell.data1);
			sell_property_to_bank(game_state, player, utility);
			break;
		}
		case generic_sell_to_bank_type::building:
			// TODO
			assert(false);
			break;
		default:
			assert(false);
			break;
		}
	}

}
