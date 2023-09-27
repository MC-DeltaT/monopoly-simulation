#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <random>

#include "player_strategy.hpp"
#include "random.hpp"
#include "simulation.hpp"
#include "statistics.hpp"


void print_statistics() {
	using namespace monopoly;

	std::cout << "Mean game length: " << statistics.rounds_mean.per_game() << "\n\n";

	std::cout << "Mean player ranks:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.player_rank_mean(player) << '\n';
	}
	std::cout << '\n';

	std::cout << "Mean final net worths:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.final_net_worth_mean(player) << '\n';
	}
	std::cout << '\n';

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
		std::cout << '\n';
	}

	std::cout << "Mean times sent to jail:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ":\t"
			<< statistics.sent_to_jail_count_mean.per_game(player) << "/game  \t"
			<< statistics.sent_to_jail_count_mean.per_turn(player) << "/turn\n";
	}
	std::cout << '\n';

	std::cout << "Mean jail duration: " << statistics.jail_duration_mean() << "\n\n";

	std::cout << "Mean rent payments:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ":\n"
			<< "    +" << statistics.rent_received_mean.per_game(player) << "/game  \t"
			<< "    +" << statistics.rent_received_mean.per_turn(player) << "/turn  \t"
			<< "    +" << statistics.rent_received_mean.per_sample(player) << "/rent\n"
			<< "    -" << statistics.rent_paid_mean.per_game(player) << "/game  \t"
			<< "    -" << statistics.rent_paid_mean.per_turn(player) << "/turn  \t"
			<< "    -" << statistics.rent_paid_mean.per_sample(player) << "/rent\n";
	}
	std::cout << '\n';

	std::cout << "Mean card cash awards:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ":\t"
			<< statistics.card_cash_award_mean.per_game(player) << "/game  \t"
			<< statistics.card_cash_award_mean.per_turn(player) << "/turn  \t"
			<< statistics.card_cash_award_mean.per_card_draw(player) << "/card\n";
	}
	std::cout << '\n';

	std::cout << "Mean card cash fees:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ":\t"
			<< statistics.card_cash_fee_mean.per_game(player) << "/game  \t"
			<< statistics.card_cash_fee_mean.per_turn(player) << "/turn  \t"
			<< statistics.card_cash_fee_mean.per_card_draw(player) << "/card\n";
	}
	std::cout << '\n';

	std::cout << "Mean cards drawn:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ":\t"
			<< statistics.cards_drawn_mean.per_game(player) << "/game  \t"
			<< statistics.cards_drawn_mean.per_turn(player) << "/turn\n";
	}
	std::cout << '\n';

	std::cout << "Simulation speed:\n"
		<< "  " << 1 / statistics.simulation_time_mean.per_game() << " game/sec\n"
		<< "  " << 1 / statistics.simulation_time_mean.per_round() << " round/sec\n"
		<< "  " << 1 / statistics.simulation_time_mean.per_turn() << " turn/sec\n"
		<< "  " << statistics.simulation_time_mean.per_game() << " sec/game\n"
		<< "  " << statistics.simulation_time_mean.per_round() << " sec/round\n"
		<< "  " << statistics.simulation_time_mean.per_turn() << " sec/turn\n";
}


int main() {
	using namespace monopoly;

	constexpr std::size_t game_count = 1000;
	constexpr auto max_rounds = 100;

	random_t random{std::random_device{}()};
	player_strategies_t strategies;

	monopoly::run_simulations(strategies, random, game_count, max_rounds);

	print_statistics();
}
