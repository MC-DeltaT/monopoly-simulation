#pragma once

#include <cassert>
#include <optional>

#include "board_space_constants.hpp"
#include "board_space_dispatch.hpp"
#include "cash.hpp"
#include "common_types.hpp"
#include "game_state.hpp"
#include "gameplay_constants.hpp"
#include "movement.hpp"
#include "player_strategy.hpp"
#include "position.hpp"
#include "random.hpp"
#include "statistics_counters.hpp"


namespace monopoly {

	inline void normal_turn(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		auto& player_state = game_state.players[player];
		assert(!player_state.in_jail());

		auto const [roll, is_double] = random.double_dice_roll();

		if (is_double) {
			auto const consecutive_doubles = player_state.consecutive_doubles + 1;
			if (consecutive_doubles >= consecutive_doubles_jail_threshold) {
				player_state.consecutive_doubles = 0;
				go_to_jail(game_state, player);
				// Turn ends.
				return;
			}
			else {
				player_state.consecutive_doubles = consecutive_doubles;
			}
		}
		else {
			player_state.consecutive_doubles = 0;
		}

		game_state.turn.movement_roll = roll;
		advance_by_spaces(game_state, strategies, random, player, roll);
		on_board_space(game_state, strategies, random, player);
	}

	inline void jail_turn(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		auto& player_state = game_state.players[player];
		assert(player_state.in_jail());

		// The rules about getting out of jail seem to be ambiguous or not well agreed upon.
		// What is implemented here is:
		// At the start of each turn in jail, the player decides what they want to do, either try to roll doubles or
		// use a Get Out of Jail Free card.
		// If the player chooses to try to roll doubles and fails, they cannot use Get Out of Jail Free on that turn.
		// On the last turn in jail, if the player chose to try to roll doubles and failed, they must pay the fine.
		// After using a Get Out of Jail Free card, the player rolls and takes their turn as normal.

		std::optional<card_type_t> used_get_out_of_jail_free_card;
		if (game_state.get_out_of_jail_free_ownership.owns_any(player)) {
			used_get_out_of_jail_free_card = strategies.visit(player, [&game_state, &random](auto& strategy) {
				return strategy.should_use_get_out_of_jail_free(game_state, random);
			});
		}

		unsigned roll;
		if (used_get_out_of_jail_free_card.has_value()) {
			assert(game_state.get_out_of_jail_free_ownership.is_owner(player, *used_get_out_of_jail_free_card));
			game_state.get_out_of_jail_free_ownership.set_owner(*used_get_out_of_jail_free_card, std::nullopt);
			roll = random.single_dice_roll();
		}
		else {
			auto const [double_roll, is_double] = random.double_dice_roll();
			if (is_double) {
				// Released from jail for free.
				roll = double_roll;
			}
			else {
				auto const new_position = player_state.position + 1;
				assert(new_position <= 0);
				if (new_position >= 0) {
					// Time in jail is up, forced to pay to be released.
					player_pay_bank(game_state, strategies, random, player, jail_release_cost);
					// May have become bankrupt from paying get out of jail fee.
					if (player_state.is_bankrupt()) {
						stat_counters.turns_in_jail[player] += max_turns_in_jail;
						// Turn ends.
						return;
					}
					roll = double_roll;
				}
				else {
					// Stil in jail.
					update_position(game_state, player, new_position);
					// Turn ends.
					return;
				}
			}
		}

		assert(player_state.position >= -static_cast<long>(max_turns_in_jail));
		assert(player_state.position < 0);
		stat_counters.turns_in_jail[player] += player_state.position + static_cast<long>(max_turns_in_jail) + 1;

		// Need to set position back to a normal board space first, since movement functions don't deal with moving
		// directly from jail.
		update_position(game_state, player, static_cast<unsigned>(board_space_t::just_visiting_jail));

		game_state.turn.movement_roll = roll;
		// It's impossible to pass Go from jail.
		assert(roll <= 12);
		advance_by_spaces_no_go(game_state, strategies, random, player, roll);
		on_board_space(game_state, strategies, random, player);
	}

	inline void do_turn(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		auto const& player_state = game_state.players[player];
		assert(!player_state.is_bankrupt());
		
		game_state.turn = turn_state_t{};

		// TODO: opportunity to build/sell houses and hotels
		// TODO: opportunity to mortgage/unmortgage properties
		// TODO: opportunity to do OTC property trades
		// TODO: opportunity to do OTC Get Out of Jail Free card trades

		if (player_state.in_jail()) {
			jail_turn(game_state, strategies, random, player);
		}
		else {
			normal_turn(game_state, strategies, random, player);
		}

		// Sanity check, player's position should always change each turn, unless they are bankrupt.
		assert(game_state.turn.position_changed || player_state.is_bankrupt());
	}

}
