#pragma once

#include <algorithm>
#include <cassert>
#include <utility>

#include "asset_surrender.hpp"
#include "cash_basic.hpp"
#include "forced_sale.hpp"
#include "game_state.hpp"
#include "gameplay_constants.hpp"
#include "player_strategy.hpp"
#include "random.hpp"
#include "safe_numeric.hpp"


namespace monopoly::detail {

	// Returns the actual cash amount a player has available to cover a payment.
	// If the player doesn't have enough cash on hand, they will be forced to sell assets.
	[[nodiscard]]
	inline unsigned generate_debit_amount(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, unsigned const amount) {
		auto const& player_cash = game_state.players[player].cash;
		if (std::cmp_greater_equal(player_cash, amount)) {
			// Enough cash on hand to pay.
			return amount;
		}
		else {
			// Not enough cash on hand - need to sell off assets.
			auto const extra_cash_needed = amount - player_cash;
			force_sell_assets(game_state, strategies, random, player, extra_cash_needed);
			// Either generated enough cash, or have to hand over everything.
			return std::min<unsigned>(player_cash, amount);
		}
	}

	// Subtract's cash from a player's balance.
	// If the player doesn't have enough cash on hand to cover the payment, they will be forced to sell assets.
	// Returns the amount which the player was able to pay (which could be less than the desired amount).
	inline unsigned raw_debit(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, unsigned const amount) {
		auto& player_state = game_state.players[player];
		// Should never be trying to debit a bankrupt player.
		assert(!player_state.is_bankrupt());
		
		auto const amount_payable = generate_debit_amount(game_state, strategies, random, player, amount);
		raw_debit_from_hand(game_state, player, amount_payable);
		if (amount_payable < amount) {
			// Couldn't cover debit, means bankruptcy.
			assert(player_state.cash == 0);
			assert(!player_state.is_bankrupt());
			player_state.bankrupt_round = game_state.round;
		}
		return amount_payable;
	}

}

namespace monopoly {

	// Player pays cash to bank. Player may have to sell assets to generate enough cash.
	inline void player_pay_bank(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, unsigned const amount) {
		detail::raw_debit(game_state, strategies, random, player, amount);

		if (game_state.players[player].is_bankrupt()) {
			surrender_assets_to_bank(game_state, player);
		}
	}

	// Player pays cash to another player. Source player may have to sell assets to generate enough cash.
	// If the source player becomes bankrupt, any remaining assets are transferred to the destination player.
	inline void player_pay_player(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const src_player, unsigned const dst_player, unsigned const amount) {
		auto const amount_yielded = detail::raw_debit(game_state, strategies, random, src_player, amount);
		detail::raw_credit(game_state, dst_player, amount_yielded);

		if (game_state.players[src_player].is_bankrupt()) {
			surrender_assets_to_player(game_state, src_player, dst_player);
		}
	}


	// Pays the player the Go salary from the bank.
	inline void pay_go_salary(game_state_t& game_state, unsigned const player) {
		bank_pay_player(game_state, player, go_salary);
	}

}
