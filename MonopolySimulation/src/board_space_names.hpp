#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include <string_view>

#include "common_constants.hpp"


namespace monopoly {

	// Gets the name of a board position.
	// 10 is "Just Visiting Jail", while negative and 40 is "In Jail".
	// This is consistent with the player position scheme and statistics space indexing.
	[[nodiscard]]
	constexpr std::string_view board_position_name(int position) {
		constexpr std::array names{
			"Go",
			"Old Kent Road",
			"Community Chest 1",
			"Whitechapel Road",
			"Income Tax",
			"Kings Cross Station",
			"The Angel, Islington",
			"Chance 1",
			"Euston Road",
			"Pentonville Road",
			"Just Visiting Jail",
			"Pall Mall",
			"Electric Company",
			"Whitehall",
			"Northumberland Avenue",
			"Marylebone Station",
			"Bow Street",
			"Community Chest 2",
			"Marlborough Street",
			"Vine Street",
			"Free Parking",
			"Strand",
			"Chance 2",
			"Fleet Street",
			"Trafalgar Square",
			"Fenchurch St Station",
			"Leicester Square",
			"Coventry Street",
			"Water Works",
			"Piccadilly",
			"Go To Jail",
			"Regent Street",
			"Oxford Street",
			"Community Chest 3",
			"Bond Street",
			"Liverpool St Station",
			"Chance 3",
			"Park Lane",
			"Super Tax",
			"Mayfair"
		};
		static_assert(names.size() == board_space_count);
		if (position < 0 || position == board_space_count) {
			return "In Jail";
		}
		else {
			return names[position];
		}
	}

}
