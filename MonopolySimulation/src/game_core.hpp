#pragma once

#include <array>
#include <limits>
#include <numeric>
#include <optional>
#include <utility>

#include "algorithm.hpp"
#include "common_constants.hpp"
#include "game_state.hpp"
#include "player_strategy.hpp"
#include "random.hpp"
#include "safe_numeric.hpp"
#include "statistics_counters.hpp"
#include "turn_logic.hpp"


namespace monopoly {

	inline std::array<unsigned, player_count> generate_player_order(random_t& random) {
		std::array<unsigned, player_count> order;
		std::iota(order.begin(), order.end(), 0u);
		fast_shuffle(order, random);
		return order;
	}

	inline void do_round(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			std::array<unsigned, player_count> const& player_order) {
		for (auto const player : player_order) {
			auto& player_state = game_state.players[player];
			if (!player_state.is_bankrupt()) {
				do_turn(game_state, strategies, random, player);
			}
		}
		safe_uint_add(game_state.round, 1u);
	}


	[[nodiscard]]
	inline bool is_game_done(game_state_t const& game_state, std::optional<unsigned> const max_rounds) noexcept {
		if (max_rounds.has_value() && std::cmp_greater_equal(game_state.round, *max_rounds)) {
			return true;
		}

		unsigned bankrupt_count = 0;
		for (auto const& player_state : game_state.players) {
			if (player_state.is_bankrupt()) {
				++bankrupt_count;
			}
		}
		return bankrupt_count + 1u >= player_count;
	}


	inline void do_game(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			std::optional<unsigned> const max_rounds = std::nullopt) {
		// Prevent overflow when game_state.round is incremented if max_rounds is large.
		static_assert(std::numeric_limits<decltype(max_rounds)::value_type>::max()
			<= std::numeric_limits<decltype(game_state_t::round)>::max());

		while (true) {
			auto const player_order = generate_player_order(random);
			do_round(game_state, strategies, random, player_order);
			if (is_game_done(game_state, max_rounds)) {
				break;
			}
		}

		if constexpr (record_stats) {
			stat_counters.games++;
			stat_counters.rounds += game_state.round;
			stat_counters.game_length_histogram.add(game_state.round);
		}
	}


	inline void reset_for_new_game(game_state_t& game_state, random_t& random) {
		game_state = game_state_t{};
		fast_shuffle(game_state.chance_deck.cards, random);
		fast_shuffle(game_state.community_chest_deck.cards, random);
	}


	inline void run_new_game(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			std::optional<unsigned> const max_rounds = std::nullopt) {
		reset_for_new_game(game_state, random);
		strategies = player_strategies_t{};
		do_game(game_state, strategies, random, max_rounds);
	}

}
