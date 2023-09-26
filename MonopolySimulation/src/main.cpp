#include <chrono>
#include <cstddef>
#include <iostream>
#include <optional>
#include <random>

#include "common_constants.hpp"
#include "game_analysis.hpp"
#include "game_core.hpp"
#include "game_state.hpp"
#include "player_strategy.hpp"
#include "random.hpp"
#include "statistics.hpp"


void print_statistics() {
	using namespace monopoly;

	std::cout << "Game length: " << statistics.game_length_mean() << std::endl;

	std::cout << "Player ranks:" << std::endl;
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.player_rank_mean(player) << std::endl;
	}

	std::cout << "Final net worths:" << std::endl;
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.final_net_worth_mean(player) << std::endl;
	}

	std::cout << "Board space frequencies:" << std::endl;
	for (unsigned space = 0; space < board_space_count + 1; ++space) {
		std::cout << "  Space " << space << ": " << statistics.board_space_relative_freq(space) << std::endl;
	}

	std::cout << "Mean times sent to jail per game:" << std::endl;
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.sent_to_jail_count_mean_per_game(player) << std::endl;
	}

	std::cout << "Mean jail duration: " << statistics.jail_duration_mean() << std::endl;

	std::cout << "Mean total rent payments per game" << std::endl;
	for (auto const player : players) {
		std::cout << "  Player " << player << ": "
			<< '+' << statistics.rent_received_mean_per_game(player) << " / "
			<< '-' << statistics.rent_paid_mean_per_game(player) << std::endl;
	}

	std::cout << "Mean total card cash awards per game" << std::endl;
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.card_cash_award_mean_per_game(player) << std::endl;
	}

	std::cout << "Mean total card cash fees per game" << std::endl;
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.card_cash_fee_mean_per_game(player) << std::endl;
	}
}


int main() {
	using namespace monopoly;

	constexpr std::size_t game_count = 1000;
	constexpr auto max_rounds = 100;

	random_t random{std::random_device{}()};
	game_state_t game_state;
	player_strategies_t strategies;

	auto const start_time = std::chrono::steady_clock::now();
	for (std::size_t g = 0; g < game_count; ++g) {
		run_new_game(game_state, strategies, random, max_rounds);
		game_end_analysis(game_state);
	}
	auto const end_time = std::chrono::steady_clock::now();

	print_statistics();

	auto const time_taken_sec = 1e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
	auto const games_per_sec = game_count / time_taken_sec;
	std::cout << games_per_sec << " games/sec" << std::endl;
}
