#pragma once

#include <array>


namespace monopoly {

	// In order of street appearance on the board.
	// Inner array indices: 0 = base, 1-4 = houses, 5 = hotel.
	// Customisable.
	inline constexpr std::array<std::array<unsigned, 6>, 22> street_rents{{
		{2, 10, 30, 90, 160, 250},
		{4, 20, 60, 180, 320, 450},
		{6, 30, 90, 270, 400, 550},
		{6, 30, 90, 270, 400, 550},
		{8, 40, 100, 300, 450, 600},
		{10, 50, 150, 450, 625, 750},
		{10, 50, 150, 450, 625, 750},
		{12, 60, 180, 500, 700, 900},
		{14, 70, 200, 550, 750, 950},
		{14, 70, 200, 550, 750, 950},
		{16, 80, 220, 600, 800, 1000},
		{18, 90, 250, 700, 875, 1050},
		{18, 90, 250, 700, 875, 1050},
		{20, 100, 300, 750, 925, 1100},
		{22, 110, 330, 800, 975, 1150},
		{22, 110, 330, 800, 975, 1150},
		{24, 120, 360, 850, 1025, 1200},
		{26, 130, 390, 900, 110, 1275},
		{26, 130, 390, 900, 1100, 1275},
		{28, 150, 450, 1000, 1200, 1400},
		{35, 175, 500, 1100, 1300, 1500},
		{50, 200, 600, 1400, 1700, 2000}
	}};

	// Customisable.
	inline constexpr unsigned full_colour_set_rent_multiplier = 2;


	// Index = number of railways owned - 1.
	// Customisable.
	inline constexpr std::array<unsigned, 4> railway_rents{25, 50, 100, 200};


	// Index = number of utilities owned - 1.
	// Customisable.
	inline constexpr std::array<unsigned, 2> utility_rent_dice_multiplier{4, 10};

}
