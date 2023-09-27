#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <numeric>
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

	std::cout << "Mean game length: " << statistics.game_length_mean() << "\n\n";

	std::cout << "Mean player ranks:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.player_rank_mean(player) << "\n";
	}
	std::cout << "\n";

	std::cout << "Mean final net worths:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.final_net_worth_mean(player) << '\n';
	}
	std::cout << "\n";

	{
		std::cout << "Board space frequencies:\n";
		auto rel_freqs = statistics.board_space_relative_freq();
		std::array<unsigned, rel_freqs.size()> board_spaces;
		std::iota(board_spaces.begin(), board_spaces.end(), 0u);
		std::ranges::sort(board_spaces, [&rel_freqs](unsigned s1, unsigned s2) {
			return rel_freqs[s1] > rel_freqs[s2];
		});
		for (auto const space : board_spaces) {
			std::cout << "  Space " << space << ": " << rel_freqs[space] << '\n';
		}
		std::cout << "\n";
	}

	std::cout << "Mean times sent to jail:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ":\t"
			<< statistics.sent_to_jail_count_mean().per_game(player) << "/g  \t"
			<< statistics.sent_to_jail_count_mean().per_turn(player) << "/t\n";
	}
	std::cout << "\n";

	std::cout << "Mean jail duration: " << statistics.jail_duration_mean() << "\n\n";

	std::cout << "Mean rent payments:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ":\n"
			<< "    +" << statistics.rent_received_mean().per_game(player) << "/g  \t"
			<< "    +" << statistics.rent_received_mean().per_turn(player) << "/t  \t"
			<< "    +" << statistics.rent_received_mean().per_sample(player) << "/s\n"
			<< "    -" << statistics.rent_paid_mean().per_game(player) << "/g  \t"
			<< "    -" << statistics.rent_paid_mean().per_turn(player) << "/t  \t"
			<< "    -" << statistics.rent_paid_mean().per_sample(player) << "/s\n";
	}
	std::cout << "\n";

	std::cout << "Mean card cash awards:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ":\t"
			<< statistics.card_cash_award_mean().per_game(player) << "/g  \t"
			<< statistics.card_cash_award_mean().per_turn(player) << "/t  \t"
			<< statistics.card_cash_award_mean().per_card_draw(player) << "/c\n";
	}
	std::cout << "\n";

	std::cout << "Mean card cash fees:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ":\t"
			<< statistics.card_cash_fee_mean().per_game(player) << "/g  \t"
			<< statistics.card_cash_fee_mean().per_turn(player) << "/t  \t"
			<< statistics.card_cash_fee_mean().per_card_draw(player) << "/c\n";
	}
	std::cout << "\n";

	std::cout << "Mean cards drawn:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ":\t"
			<< statistics.cards_drawn_mean().per_game(player) << "/g  \t"
			<< statistics.cards_drawn_mean().per_turn(player) << "/t\n";
	}
	std::cout << "\n";
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
