#pragma once

#include <cassert>
#include <optional>
#include <utility>

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
		auto const prev_top_index = deck.top_index;
		auto card = deck.next_card();
		if (card == get_out_of_jail_free_card<C>) {
			if (game_state.get_out_of_jail_free_ownership.is_owned(C)) {
				// If the card is Get Out Of Jail Free and it's already taken, try again.
				card = deck.next_card();
				assert(card != get_out_of_jail_free_card<C>);
			}
			else {
				assert(!deck.get_out_of_jail_free_index.has_value());
				deck.get_out_of_jail_free_index = prev_top_index;
			}
		}
		return card;
	}

	// Returns a Get Out Of Jail Free card from a player's ownership to the card deck.
	template<card_type_t C>
	void return_get_out_of_jail_free_card(game_state_t& game_state) {
		auto& deck = game_state.card_deck<C>();
		assert(game_state.get_out_of_jail_free_ownership.is_owned(C));
		game_state.get_out_of_jail_free_ownership.set_owner(C, std::nullopt);

		assert(deck.get_out_of_jail_free_index.has_value());
		auto const old_card_idx = *deck.get_out_of_jail_free_index;
		assert(deck.cards[old_card_idx] == get_out_of_jail_free_card<C>);
		if (deck.top_index == old_card_idx) {
			// If Get Out Of Jail Free is the next card, just need to skip it and it'll be at the back.
			deck.inc_top_index();
		}
		else {
			// Shuffle Get Out Of Jail Free card back until it's at the end of the deck.
			auto prev = old_card_idx;
			for (unsigned i = (old_card_idx + 1) % deck.size; i != deck.top_index; i = (i + 1) % deck.size) {
				std::swap(deck.cards[prev], deck.cards[i]);
				prev = i;
			}
			assert((prev + 1) % deck.size == deck.top_index);
		}
		assert(deck.cards[(deck.top_index + deck.size - 1) % deck.size] == get_out_of_jail_free_card<C>);

#ifndef NDEBUG
		deck.get_out_of_jail_free_index = std::nullopt;
#endif
	}

}
