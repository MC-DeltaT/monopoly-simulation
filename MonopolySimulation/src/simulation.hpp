#pragma once

#include <chrono>
#include <cstddef>
#include <optional>

#include "game_analysis.hpp"
#include "game_core.hpp"
#include "game_state.hpp"
#include "player_strategy.hpp"
#include "random.hpp"
#include "statistics_counters.hpp"


namespace monopoly {

	// Runs a number of games for the purposes of collecting statistics.
	inline void run_simulations(player_strategies_t& strategies, random_t& random, std::size_t const game_count,
			std::optional<unsigned> const max_rounds = std::nullopt) {
		game_state_t game_state;

		auto const start_time = std::chrono::steady_clock::now();
		for (std::size_t g = 0; g < game_count; ++g) {
			stat_helper_state = stat_helper_state_t{};
			run_new_game(game_state, strategies, random, max_rounds);
			game_end_analysis(game_state);
		}
		auto const end_time = std::chrono::steady_clock::now();

		using float_seconds = std::chrono::duration<double>;
		stat_counters.simulation_time_seconds =
			std::chrono::duration_cast<float_seconds>(end_time - start_time).count();
	}

}
