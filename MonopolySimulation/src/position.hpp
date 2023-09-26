#pragma once

#include <cassert>
#include <utility>

#include "common_constants.hpp"
#include "game_state.hpp"
#include "statistics.hpp"


namespace monopoly {

	// Updates the player's position to the specified value.
	inline void update_position(game_state_t& game_state, unsigned const player, int const position) {
		auto& player_state = game_state.players[player];
		// Probably shouldn't be trying to set position to the current position.
		assert(player_state.position != position);
		assert(std::cmp_less(position, board_space_count));

		player_state.position = position;

		game_state.turn.position_changed = true;

		if (position >= 0) {
			++statistics.board_space_counts[position];
		}
		else {
			// Jail.
			++statistics.board_space_counts[40];
		}
		++statistics.position_count;
	}

	// Advance the player's composition by a number of spaces relative to the current position.
	// Doesn't do anything else, e.g. handling of the board space.
	// Return value indicated is passed Go.
	[[nodiscard]]
	inline bool advance_position_relative(game_state_t& game_state, unsigned const player, unsigned const offset) {
		// Trying to advance by 0 is probably a bug somewhere.
		assert(offset > 0);
		// Should never be advancing all the way around the board to or past the current position.
		assert(std::cmp_less(offset, board_space_count));
		auto const player_position = game_state.players[player].position;
		assert(player_position >= 0 && std::cmp_less(player_position, board_space_count));

		auto const new_position = static_cast<unsigned>(player_position) + offset;
		if (new_position < board_space_count) {
			// Didn't pass go.
			update_position(game_state, player, new_position);
			return false;
		}
		else {
			// Passed Go.
			// Since you cannot possibly move around the whole board, the index must be in the range [40, 80).
			assert(new_position >= board_space_count && new_position < 2u * board_space_count);
			update_position(game_state, player, new_position - board_space_count);
			return true;
		}
	}

	// Advance the player's position to the specified board space index.
	// Doesn't do anything else, e.g. handling of the board space.
	// Return value indicates if passed Go.
	[[nodiscard]]
	inline bool advance_position_absolute(game_state_t& game_state, unsigned const player, unsigned const new_index) {
		assert(new_index < board_space_count);		// Must be a valid board index.
		auto const& player_state = game_state.players[player];
		assert(player_state.position >= 0 && player_state.position < board_space_count);
		assert(std::cmp_not_equal(player_state.position, new_index));

		auto const prev_position = player_state.position;
		update_position(game_state, player, new_index);

		// If the new board index is lower than the previous board index, must have passed Go.
		return std::cmp_less(new_index, prev_position);
	}

}
