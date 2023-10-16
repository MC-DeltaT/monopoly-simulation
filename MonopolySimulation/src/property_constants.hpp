#pragma once

#include <array>
#include <ranges>

#include "common_types.hpp"


namespace monopoly {

	// In order of street appearance on the board.
	// generic_index must be sequential starting from 0.
	inline constexpr std::array<street_t, 22> streets{{
		{0, 0, 0},
		{1, 0, 1},
		{2, 1, 0},
		{3, 1, 1},
		{4, 1, 2},
		{5, 2, 0},
		{6, 2, 1},
		{7, 2, 2},
		{8, 3, 0},
		{9, 3, 1},
		{10, 3, 2},
		{11, 4, 0},
		{12, 4, 1},
		{13, 4, 2},
		{14, 5, 0},
		{15, 5, 1},
		{16, 5, 2},
		{17, 6, 0},
		{18, 6, 1},
		{19, 6, 2},
		{20, 7, 0},
		{21, 7, 1},
	}};
	
	inline constexpr unsigned colour_set_count = streets.back().colour_set + 1;

	inline constexpr std::array<unsigned, colour_set_count> colour_set_sizes = []{
		std::array<unsigned, colour_set_count> result{};
		for (auto const& street : streets) {
			result[street.colour_set] = std::max(street.index_in_set + 1u, result[street.colour_set]);
		}
		return result;
	}();

	inline constexpr unsigned max_colour_set_size = std::ranges::max(colour_set_sizes);


	// These must be sequential starting from 0.
	enum class railway_t : unsigned {
		kings_cross,
		marylebone,
		fenchurch_street,
		liverpool_street
	};

	inline constexpr std::array<railway_t, 4> railways{
		railway_t::kings_cross,
		railway_t::marylebone,
		railway_t::fenchurch_street,
		railway_t::liverpool_street
	};


	// These must be sequential starting from 0.
	enum class utility_t : unsigned {
		electric_company,
		water_works
	};

	inline constexpr std::array<utility_t, 2> utilities{
		utility_t::electric_company,
		utility_t::water_works
	};


	inline constexpr street_t::street_t(unsigned const generic_index) :
		street_t{streets[generic_index]}
	{}
	
}
