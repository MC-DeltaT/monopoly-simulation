#pragma once

#include <cassert>
#include <optional>

#include "board_space_constants.hpp"
#include "board_space_dispatch.hpp"
#include "card_deck_operations.hpp"
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

	// Return value indicates if the player gets another turn due to rolling doubles.
	[[nodiscard]]
	inline bool normal_turn(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		auto& player_state = game_state.players[player];
		assert(!player_state.in_jail());
		assert(!player_state.is_bankrupt());

		auto const [roll, is_double] = random.double_dice_roll();

		if (is_double) {
			auto const consecutive_doubles = player_state.consecutive_doubles + 1;
			if (consecutive_doubles >= consecutive_doubles_jail_threshold) {
				player_state.consecutive_doubles = 0;
				go_to_jail(game_state, player);
				// Turn ends.
				return false;
			}
			else {
				player_state.consecutive_doubles = consecutive_doubles;
			}
		}
		else {
			player_state.consecutive_doubles = 0;
		}

		game_state.turn.movement_roll = roll;
		advance_by_spaces(game_state, player, roll);
		on_board_space(game_state, strategies, random, player);

		// Presumably if the player is sent to jail they don't get another turn.
		return is_double && !player_state.in_jail() && !player_state.is_bankrupt();
	}

	inline void jail_turn(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		auto& player_state = game_state.players[player];
		assert(player_state.in_jail());
		assert(!player_state.is_bankrupt());

		// The rules about getting out of jail seem to be ambiguous or not well agreed upon.
		// What is implemented here is as follows.
		// At the start of each turn in jail, the player decides what they want to do:
		//   - pay fine
		//   - use Get Out Of Jail Free card
		//   - try to roll doubles
		// After choosing to pay the fine or use a Get Out Of Jail Free card, the roll and move immediately.
		// If choosing to roll doubles:
		//   - if successful, use that roll to move
		//   - if unsuccessful:
		//       - if last turn allowable turn in jail, pay fine and use that roll to move
		//       - else forfeit turn

		auto const jail_action = strategies.visit(player,
			[&game_state, &random](PlayerStrategy auto& strategy) {
				return strategy.decide_jail_action(game_state, random);
			});

		unsigned roll;

		auto const use_get_out_of_jail_free_card = [&game_state, &random, player, &roll]<card_type_t C>() {
			assert(game_state.get_out_of_jail_free_ownership.is_owner(player, C));
			return_get_out_of_jail_free_card<C>(game_state);
			roll = random.single_dice_roll();
		};

		switch (jail_action) {
		case in_jail_action_t::pay_fine: {
			player_pay_bank_from_hand(game_state, player, jail_release_cost);
			if constexpr (record_stats) {
				stat_counters.jail_fee_paid_count[player]++;
			}
			break;
		}

		case in_jail_action_t::get_out_of_jail_free_chance:
			use_get_out_of_jail_free_card.operator()<card_type_t::chance>();
			break;

		case in_jail_action_t::get_out_of_jail_free_community_chest:
			use_get_out_of_jail_free_card.operator()<card_type_t::community_chest>();
			break;

		case in_jail_action_t::roll_doubles: {
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
					if constexpr (record_stats) {
						stat_counters.jail_fee_paid_count[player]++;
					}

					// May have become bankrupt from paying get out of jail fee.
					if (player_state.is_bankrupt()) {
						if constexpr (record_stats) {
							stat_counters.turns_in_jail[player] += max_turns_in_jail;
						}
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
			break;
		}

		default:
			assert(false);
			break;
		}

		// If we get here then player is being released from jail.

		if constexpr (record_stats) {
			assert(std::cmp_greater_equal(player_state.position, -static_cast<long>(max_turns_in_jail)));
			assert(std::cmp_less(player_state.position, 0));
			stat_counters.turns_in_jail[player] += player_state.position + static_cast<long>(max_turns_in_jail) + 1;
		}

		// Need to set position back to a normal board space first, since movement functions don't deal with moving
		// directly from jail.
		update_position(game_state, player, static_cast<unsigned>(board_space_t::just_visiting_jail));

		game_state.turn.movement_roll = roll;
		// It's impossible to pass Go from jail.
		assert(roll <= 12);
		advance_by_spaces_no_go(game_state, player, roll);
		on_board_space(game_state, strategies, random, player);
	}

	// Return value indicates if the player gets another turn due to rolling doubles.
	[[nodiscard]]
	inline bool do_single_turn(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		auto const& player_state = game_state.players[player];
		assert(!player_state.is_bankrupt());
		
		game_state.turn = turn_state_t{};

		// TODO: opportunity to build/sell houses and hotels
		// TODO: opportunity to mortgage/unmortgage properties
		// TODO: opportunity to do OTC property trades
		// TODO: opportunity to do OTC Get Out of Jail Free card trades

		bool extra_turn = false;
		if (player_state.in_jail()) {
			jail_turn(game_state, strategies, random, player);
		}
		else {
			extra_turn = normal_turn(game_state, strategies, random, player);
		}

		// Sanity check, player's position should always change each turn, unless they are bankrupt.
		assert(game_state.turn.position_changed || player_state.is_bankrupt());

		if constexpr (record_stats) {
			stat_counters.turns_played[player]++;
		}

		return extra_turn;
	}

	inline void do_turn(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		while (true) {
			auto const extra_turn = do_single_turn(game_state, strategies, random, player);
			if (!extra_turn) {
				break;
			}
		}
	}

}
