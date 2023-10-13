#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include <string_view>

#include "algorithm.hpp"
#include "common_constants.hpp"


namespace monopoly {

	inline constexpr std::array<std::string_view, street_count> street_names{
		"Old Kent Road",
		"Whitechapel Road",
		"The Angel, Islington",
		"Euston Road",
		"Pentonville Road",
		"Pall Mall",
		"Whitehall",
		"Northumberland Avenue",
		"Bow Street",
		"Marlborough Street",
		"Vine Street",
		"Strand",
		"Fleet Street",
		"Trafalgar Square",
		"Leicester Square",
		"Coventry Street",
		"Piccadilly",
		"Regent Street",
		"Oxford Street",
		"Bond Street",
		"Park Lane",
		"Mayfair"
	};
	static_assert(all_unique(street_names));

	inline constexpr std::array<std::string_view, railway_count> railway_names{
		"Kings Cross Station",
		"Marylebone Station",
		"Fenchurch St Station",
		"Liverpool St Station"
	};
	static_assert(all_unique(railway_names));

	inline constexpr std::array<std::string_view, utility_count> utility_names{
		"Electric Company",
		"Water Works"
	};
	static_assert(all_unique(utility_names));

	// Gets the name of a board position.
	// 10 is "Just Visiting Jail", while negative and 40 is "In Jail".
	// This is consistent with the player position scheme and statistics space indexing.
	[[nodiscard]]
	constexpr std::string_view board_position_name(int position) {
		constexpr std::array<std::string_view, board_space_count> names{
			"Go",
			std::get<0>(street_names),
			"Community Chest 1",
			std::get<1>(street_names),
			"Income Tax",
			std::get<0>(railway_names),
			std::get<2>(street_names),
			"Chance 1",
			std::get<3>(street_names),
			std::get<4>(street_names),
			"Just Visiting Jail",
			std::get<5>(street_names),
			std::get<0>(utility_names),
			std::get<6>(street_names),
			std::get<7>(street_names),
			std::get<1>(railway_names),
			std::get<8>(street_names),
			"Community Chest 2",
			std::get<9>(street_names),
			std::get<10>(street_names),
			"Free Parking",
			std::get<11>(street_names),
			"Chance 2",
			std::get<12>(street_names),
			std::get<13>(street_names),
			std::get<2>(railway_names),
			std::get<14>(street_names),
			std::get<15>(street_names),
			std::get<1>(utility_names),
			std::get<16>(street_names),
			"Go To Jail",
			std::get<17>(street_names),
			std::get<18>(street_names),
			"Community Chest 3",
			std::get<19>(street_names),
			std::get<3>(railway_names),
			"Chance 3",
			std::get<20>(street_names),
			"Super Tax",
			std::get<21>(street_names),
		};
		static_assert(all_unique(names));

		if (position < 0 || position == board_space_count) {
			return "In Jail";
		}
		else {
			return names[position];
		}
	}

}
