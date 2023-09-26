#pragma once

#include <cassert>

#include "game_state.hpp"
#include "safe_numeric.hpp"


namespace monopoly::detail {

	// Adds cash to a player's balance.
	inline void raw_credit(game_state_t& game_state, unsigned const player, unsigned const amount) {
		auto& player_state = game_state.players[player];
		// Should never be receiving cash if bankrupt.
		assert(!player_state.is_bankrupt());
		safe_uint_add(player_state.cash, amount);
	}

	// Subtract's cash from a player's balance. Assumes that the player has enough cash on hand.
	inline void raw_debit_from_hand(game_state_t& game_state, unsigned const player, unsigned const amount) {
		auto& player_state = game_state.players[player];
		// Should never be trying to debit a bankrupt player.
		assert(!player_state.is_bankrupt());
		safe_uint_sub(player_state.cash, amount);
	}

}

namespace monopoly {

	// Bank pays cash to player.
	inline void bank_pay_player(game_state_t& game_state, unsigned const player, unsigned const amount) {
		detail::raw_credit(game_state, player, amount);
	}

	// Player pays cash to bank. Assumes that the player has enough cash on hand.
	inline void player_pay_bank_from_hand(game_state_t& game_state, unsigned const player, unsigned const amount) {
		detail::raw_debit_from_hand(game_state, player, amount);
	}

}
