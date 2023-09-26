#pragma once

#include <cassert>

#include "common_types.hpp"
#include "game_state.hpp"


namespace monopoly {


	// Checks if a street can be mortgaged.
	[[nodiscard]]
	inline bool is_street_mortgageable(game_state_t const& game_state, street_t const street) {
		return
		// Can't mortgage a property that's already mortgaged or has buildings.
			game_state.street_development.development_level(street) == 0
		// Can't mortgage a street if there are any buildings in that colour set.
			&& !game_state.street_development.colour_set_has_buildings(street.colour_set);
	}


	// Checks if a street can be sold, assuming it is currently owned by a player.
	[[nodiscard]]
	inline bool is_property_sellable(game_state_t const& game_state, street_t const street) {
		return
		// Can't sell a mortgaged property.
			!game_state.street_development.is_mortgaged(street)
		// Can't sell a street with buildings (must sell buildings first).
			&& game_state.street_development.building_level(street) == 0
		// Can't sell if the colour set has any buildings.
			&& !game_state.street_development.colour_set_has_buildings(street.colour_set);
	}

	// Checks if a railway can be sold, assuming it is currently owned by a player.
	[[nodiscard]]
	inline bool is_property_sellable(game_state_t const& game_state, railway_t const railway) {
		// Can't sell a mortgaged property.
		return !game_state.railway_development.is_mortgaged(railway);
	}

	// Checks if a utility can be sold, assuming it is currently owned by a player.
	[[nodiscard]]
	inline bool is_property_sellable(game_state_t const& game_state, utility_t const utility) {
		// Can't sell a mortgaged property.
		return !game_state.utility_development.is_mortgaged(utility);
	}


	// Checks if 1 more building can be built on a street, assuming the street is currently owned by a player.
	// Considers only the state of the street and its colour set, not whether there are enough available houses/hotels,
	// nor whether a player has enough money to purchase a house/hotel.
	[[nodiscard]]
	inline bool is_street_buildable(game_state_t const& game_state, street_t const street) {
		auto const development_level = game_state.street_development.development_level(street);
		auto const min_development_in_set =
			game_state.street_development.min_development_level_in_set(street.colour_set);
		// Should never have more than 1 building more than any other street in that colour set.
		assert(development_level >= min_development_in_set
			&& development_level - min_development_in_set <= 1);

		return
		// Can't build on a mortgaged property.
			development_level >= 0
		// Can build at most 1 hotel.
			&& development_level <= 5
		// Can't add a building if it would cause an uneven distribution of buildings within the colour set.
			&& development_level == min_development_in_set;
	}


	// Checks if 1 building can be removed from a street.
	[[nodiscard]]
	inline bool is_building_removable(game_state_t const& game_state, street_t const street) {
		auto const development_level = game_state.street_development.development_level(street);
		auto const max_development_in_set =
			game_state.street_development.max_development_level_in_set(street.colour_set);
		// Should never have more than 1 building less than any other street in that colour set.
		assert(development_level <= max_development_in_set
			&& max_development_in_set - development_level <= 1);

		return
		// Can't sell if there are no buildings.
			development_level > 0
		// Can't remove a building if it would cause an uneven distribution of buildings within the colour set.
			&& development_level == max_development_in_set;
	}

}
