#pragma once

#include <array>


namespace monopoly {

	// These must be sequential starting from 0.
	// Very much not customisable (things will break!).
	enum class board_space_t : unsigned {
		go,
		old_kent_road,
		community_chest_1,
		whitechapel_road,
		income_tax,
		kings_cross_station,
		the_angel_islington,
		chance_1,
		euston_road,
		pentonville_road,
		just_visiting_jail,
		pall_mall,
		electric_company,
		whitehall,
		northumberland_avenue,
		marylebone_station,
		bow_street,
		community_chest_2,
		marlborough_street,
		vine_street,
		free_parking,
		strand,
		chance_2,
		fleet_street,
		trafalgar_square,
		fenchurch_street_station,
		leicester_square,
		coventry_street,
		water_works,
		piccadilly,
		go_to_jail,
		regent_street,
		oxford_street,
		community_chest_3,
		bond_street,
		liverpool_street_station,
		chance_3,
		park_lane,
		super_tax,
		mayfair
	};

	// In order of appearance on the board.
	inline constexpr std::array<board_space_t, 4> railway_spaces{
		board_space_t::kings_cross_station,
		board_space_t::marylebone_station,
		board_space_t::fenchurch_street_station,
		board_space_t::liverpool_street_station,
	};

	// In order of appearance on the board.
	inline constexpr std::array<board_space_t, 2> utility_spaces{
		board_space_t::electric_company,
		board_space_t::water_works
	};


	// Customisable.
	inline constexpr unsigned income_tax = 200;

	// Customisable.
	inline constexpr unsigned super_tax = 150;

}
