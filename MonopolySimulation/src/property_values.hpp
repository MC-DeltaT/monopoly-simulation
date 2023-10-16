#pragma once

#include <array>

#include "common_types.hpp"


namespace monopoly {

	// In order of street appearance on the board.
	// Customisable.
	inline constexpr std::array<unsigned, 22> street_values{
		60, 60,
		100, 100, 120,
		140, 140, 160,
		180, 180, 200,
		220, 220, 240,
		260, 260, 280,
		300, 300, 320,
		350, 400
	};

	// For each colour set. Houses and hotels have the same value.
	// Customisable.
	inline constexpr std::array<unsigned, 8> building_values{50, 60, 100, 100, 150, 150, 200, 200};


	// Customisable.
	inline constexpr unsigned railway_value = 200;


	// Customisable.
	inline constexpr unsigned utility_value = 200;


	[[nodiscard]]
	constexpr unsigned property_buy_cost(street_t const street) {
		return street_values[street.generic_index];
	}

	[[nodiscard]]
	constexpr unsigned property_buy_cost(railway_t) noexcept {
		return railway_value;
	}

	[[nodiscard]]
	constexpr unsigned property_buy_cost(utility_t) noexcept {
		return utility_value;
	}


	constexpr unsigned railway_mortgage_value = railway_value / 2u;

	constexpr unsigned utility_mortgage_value = utility_value / 2u;

	[[nodiscard]]
	constexpr unsigned property_mortgage_value(street_t const street) {
		return street_values[street.generic_index] / 2u;
	}

	[[nodiscard]]
	constexpr unsigned property_mortgage_value(railway_t) noexcept {
		return railway_mortgage_value;
	}

	[[nodiscard]]
	constexpr unsigned property_mortgage_value(utility_t) noexcept {
		return utility_mortgage_value;
	}


	constexpr unsigned railway_sell_value = railway_value / 2u;

	constexpr unsigned utility_sell_value = utility_value / 2u;

	[[nodiscard]]
	constexpr unsigned property_sell_value(street_t const street) {
		return street_values[street.generic_index] / 2u;
	}

	[[nodiscard]]
	constexpr unsigned property_sell_value(railway_t) noexcept {
		return railway_sell_value;
	}

	[[nodiscard]]
	constexpr unsigned property_sell_value(utility_t) noexcept {
		return utility_sell_value;
	}

	[[nodiscard]]
	constexpr unsigned building_sell_value(street_t const street) {
		return building_values[street.colour_set] / 2u;
	}

}
