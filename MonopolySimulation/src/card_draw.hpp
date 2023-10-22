#pragma once

#include <cassert>

#include "card_constants.hpp"
#include "common_types.hpp"
#include "game_state.hpp"


namespace monopoly {

	// Takes the next card from the Chance or Community Chest deck.
	// Updates the deck state but doesn't do anything else.
	template<card_type_t C>
	[[nodiscard]]
	auto draw_card(game_state_t& game_state) {
		auto& deck = game_state.card_deck<C>();
		auto const is_get_out_of_jail_free_owned = game_state.get_out_of_jail_free_ownership.is_owned(C);
		auto const card = deck.draw_card(is_get_out_of_jail_free_owned);
		assert(!is_get_out_of_jail_free_owned || card != get_out_of_jail_free_card<C>);
		return card;
	}

}
