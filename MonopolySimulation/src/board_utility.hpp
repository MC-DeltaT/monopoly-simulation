#pragma once

#include "board_space_constants.hpp"
#include "common_types.hpp"


namespace monopoly {

	// Gets the next railway space after the specified board space.
	[[nodiscard]]
	constexpr board_space_t next_railway_lookup(board_space_t const space) noexcept {
		for (auto const railway : railway_spaces) {
			if (space < railway) {
				return railway;
			}
		}
		return std::get<0>(railway_spaces);
	}

	// Gets next utility space after the specified board space.
	[[nodiscard]]
	constexpr board_space_t next_utility_lookup(board_space_t const space) noexcept {
		for (auto const utility : utility_spaces) {
			if (space < utility) {
				return utility;
			}
		}
		return std::get<0>(utility_spaces);
	}

}
