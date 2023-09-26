#pragma once

#include <cassert>

#include "card_constants.hpp"
#include "common_types.hpp"
#include "game_state.hpp"
#include "property_constants.hpp"


namespace monopoly::detail {

	template<PropertyType P>
	void surrender_property_to_bank(game_state_t& game_state, unsigned const player, P const property) {
		if (game_state.property_ownership<P>().is_owner(player, property)) {
			// If property isn't mortgaged then it should've been liquidated previously.
			assert(game_state.property_development<P>().is_mortgaged(property));
			game_state.property_ownership<P>().set_owner(property, std::nullopt);
		}
	}

	template<PropertyType P>
	void surrender_property_to_player(game_state_t& game_state, unsigned const src_player, P const property,
			unsigned const dst_player) {
		if (game_state.property_ownership<P>().is_owner(src_player, property)) {
			// If property isn't mortgaged then it should've been liquidated previously.
			assert(game_state.property_development<P>().is_mortgaged(property));
			game_state.property_ownership<P>().set_owner(property, dst_player);
			// TODO: decide to unmortgage property or not
			// TODO: if not unmortgage, pay interest
		}
	}

}

namespace monopoly {

	void surrender_assets_to_bank(game_state_t& game_state, unsigned const player) {
		// Should not have any buildings (they would be liquidated previously)
		// Should not have unmortgaged properties (they would be liquidated previously)
		// Might have mortgaged properties - streets, railways, utilities
		// Shouldn't have cash (would be turned over previously)

		// TODO: auction surrendered properties

		for (auto const& street : streets) {
			detail::surrender_property_to_bank(game_state, player, street);
		}

		for (auto const utility : utilities) {
			detail::surrender_property_to_bank(game_state, player, utility);
		}

		for (auto const railway : railways) {
			detail::surrender_property_to_bank(game_state, player, railway);
		}

		for (auto const card_type : card_types) {
			if (game_state.get_out_of_jail_free_ownership.is_owner(player, card_type)) {
				game_state.get_out_of_jail_free_ownership.set_owner(card_type, std::nullopt);
			}
		}

		assert(game_state.players[player].cash == 0);
	}

	void surrender_assets_to_player(game_state_t& game_state, unsigned const src_player, unsigned const dst_player) {
		// Should not have any buildings (they would be liquidated previously)
		// Should not have unmortgaged properties (they would be liquidated previously)
		// Might have mortgaged properties - streets, railways, utilities
		// Shouldn't have cash (would be turned over previously)

		for (auto const& street : streets) {
			detail::surrender_property_to_player(game_state, src_player, street, dst_player);
		}

		for (auto const utility : utilities) {
			detail::surrender_property_to_player(game_state, src_player, utility, dst_player);
		}

		for (auto const railway : railways) {
			detail::surrender_property_to_player(game_state, src_player, railway, dst_player);
		}

		for (auto const card_type : card_types) {
			if (game_state.get_out_of_jail_free_ownership.is_owner(src_player, card_type)) {
				game_state.get_out_of_jail_free_ownership.set_owner(card_type, dst_player);
			}
		}

		assert(game_state.players[src_player].cash == 0);
	}

}
