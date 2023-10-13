#pragma once

#include <cassert>
#include <utility>

#include "board_space_constants.hpp"
#include "cash.hpp"
#include "game_state.hpp"
#include "gameplay_constants.hpp"
#include "player_strategy.hpp"
#include "position.hpp"
#include "random.hpp"
#include "statistics_counters.hpp"


namespace monopoly {

	// Advances the player's position to the specified board space, paying the Go salary if passing Go.
	// Not to be used for advancing to Go, use advance_to_go() instead.
	inline void advance_to_space(game_state_t& game_state, unsigned const player, board_space_t const space) {
		// Can't use this to advance to Go, because Go is handled separately.
		assert(space != board_space_t::go);

		auto const passed_go = advance_position_absolute(game_state, player, static_cast<unsigned>(space));
		if (passed_go) {
			pay_go_salary(game_state, player);
		}
	}

	// Advances the player's position by a number of board spaces, paying the Go salary if passing Go.
	inline void advance_by_spaces(game_state_t& game_state, unsigned const player, unsigned const offset) {
		auto const passed_go = advance_position_relative(game_state, player, offset);
		if (passed_go) {
			pay_go_salary(game_state, player);
		}
	}

	// Advances the player's position by a number of board spaces.
	// Assumes the movement will not advance the player past Go (i.e. never pays the Go salary).
	inline void advance_by_spaces_no_go(game_state_t& game_state, unsigned player, unsigned const offset) {
		[[maybe_unused]] auto const passed_go = advance_position_relative(game_state, player, offset);
		assert(!passed_go);
	}

	// Moves the player's position back by a number of board spaces.
	// Cannot be used to move backwards through Go.
	inline void retreat_by_spaces(game_state_t& game_state, unsigned const player, unsigned const offset) {
		auto const player_position = game_state.players[player].position;
		assert(std::cmp_greater(player_position, offset));
		auto const new_position = static_cast<unsigned>(player_position) - offset;
		update_position(game_state, player, new_position);
	}


	// Advances the player's position to Go and pays them the salary.
	inline void advance_to_go(game_state_t& game_state, unsigned const player) {
		update_position(game_state, player, static_cast<unsigned>(board_space_t::go));
		pay_go_salary(game_state, player);
	}

	// Moves the player directly to jail, without passing Go.
	inline void go_to_jail(game_state_t& game_state, unsigned const player) {
		// Position starts negative and counts up to 0 each turn in jail.
		constexpr auto in_jail_initial_position = -static_cast<int>(max_turns_in_jail);
		update_position(game_state, player, in_jail_initial_position);

		if constexpr (record_stats) {
			stat_counters.sent_to_jail_count[player]++;
		}
	}

}
