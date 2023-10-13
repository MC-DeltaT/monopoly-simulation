#pragma once

#include <cassert>

#include "board_utility.hpp"
#include "cash.hpp"
#include "common_constants.hpp"
#include "common_types.hpp"
#include "movement.hpp"
#include "statistics_counters.hpp"


namespace monopoly {

	void on_board_space(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
		unsigned const player);

}

namespace monopoly::card_effects {

	inline void cash_award(game_state_t& game_state, unsigned const player, unsigned const amount) {
		bank_pay_player(game_state, player, amount);

		if constexpr (record_stats) {
			stat_counters.cash_award_card_amount[player] += amount;
			stat_counters.cash_award_cards_drawn[player]++;
		}

		// Turn ends.
	}

	inline void cash_fee(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, unsigned const amount) {
		player_pay_bank(game_state, strategies, random, player, amount);

		if constexpr (record_stats) {
			stat_counters.cash_fee_card_amount[player] += amount;
			stat_counters.cash_fee_cards_drawn[player]++;
		}

		// Turn ends.
	}

	inline void per_building_cash_fee(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, unsigned const amount_per_house, unsigned const amount_per_hotel) {
		auto const& player_state = game_state.players[player];
		auto const amount = amount_per_house * player_state.houses_owned + amount_per_hotel * player_state.hotels_owned;
		cash_fee(game_state, strategies, random, player, amount);
		// Turn ends.
	}

	inline void cash_award_from_players(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, unsigned const amount) {
		for (auto const other_player : players) {
			if (other_player != player && !game_state.players[other_player].is_bankrupt()) {
				player_pay_player(game_state, strategies, random, other_player, player, amount);
			}
		}
		// Turn ends.
	}

	inline void cash_fee_to_players(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, unsigned const amount) {
		for (auto const other_player : players) {
			if (other_player != player && !game_state.players[other_player].is_bankrupt()) {
				player_pay_player(game_state, strategies, random, player, other_player, amount);
				// If the player goes bankrupt, don't keep trying to pay other players.
				if (game_state.players[player].is_bankrupt()) {
					break;
				}
			}
		}
		// Turn ends.
	}


	inline void advance_to_go(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		monopoly::advance_to_go(game_state, player);
		on_board_space(game_state, strategies, random, player);
		// Turn ends.
	}

	inline void go_to_jail(game_state_t& game_state, unsigned const player) {
		monopoly::go_to_jail(game_state, player);
		// Turn ends.
	}

	inline void go_back_3_spaces(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		// Note that no Chance space would make it possible to go backwards past Go.
		retreat_by_spaces(game_state, player, 3);
		on_board_space(game_state, strategies, random, player);
	}

	inline void advance_to_space(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, board_space_t const space) {
		monopoly::advance_to_space(game_state, player, space);
		on_board_space(game_state, strategies, random, player);
	}

	inline void advance_to_next_railway(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		game_state.turn.railway_rent_multiplier = 2;
		auto const current_space = game_state.players[player].get_board_space();
		auto const next_railway = next_railway_lookup(current_space);
		monopoly::advance_to_space(game_state, player, next_railway);
		on_board_space(game_state, strategies, random, player);
	}

	inline void advance_to_next_utility(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		game_state.turn.utility_rent_dice_multiplier_override = 10;
		auto const current_space = game_state.players[player].get_board_space();
		auto const next_utility = next_utility_lookup(current_space);
		monopoly::advance_to_space(game_state, player, next_utility);
		on_board_space(game_state, strategies, random, player);
	}


	inline void receive_get_out_of_jail_free(game_state_t& game_state, unsigned const player,
			card_type_t const card) {
		assert(!game_state.get_out_of_jail_free_ownership.is_owned(card));
		game_state.get_out_of_jail_free_ownership.set_owner(card, player);
		// Turn ends.
	}

}
