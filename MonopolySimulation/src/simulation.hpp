#pragma once

#include <chrono>
#include <cstddef>
#include <optional>

#include "game_analysis.hpp"
#include "game_core.hpp"
#include "game_state.hpp"
#include "multithreading.hpp"
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

		if constexpr (record_stats) {
			using float_seconds = std::chrono::duration<double>;
			stat_counters.simulation_time_seconds =
				std::chrono::duration_cast<float_seconds>(end_time - start_time).count();
		}
	}

	inline void run_simulations_multithreaded(auto strategies_factory, auto random_factory, std::size_t game_count,
			std::optional<unsigned> const max_rounds = std::nullopt, std::optional<unsigned> threads = std::nullopt) {
		if (!threads.has_value()) {
			auto const hw_threads = std::thread::hardware_concurrency();
			if (hw_threads > 0) {
				threads = hw_threads;
			}
			else {
				threads = 4;
			}
		}

		const auto games_per_thread = game_count / threads.value();

		auto const thread_func = [strategies_factory, random_factory, max_rounds, games_per_thread]
				(stat_counters_t& result) {
			random_t random{random_factory()};
			player_strategies_t strategies{strategies_factory()};

			run_simulations(strategies, random, games_per_thread, max_rounds);

			result = stat_counters;
		};

		// Statistics counters from threads are accumulated into the main thread's counters.
		stat_counters = map_multithreaded<stat_counters_t>(thread_func, threads.value());
	}

}
