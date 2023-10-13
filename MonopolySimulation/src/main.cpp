#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <ranges>

#include "algorithm.hpp"
#include "board_space_names.hpp"
#include "common_types.hpp"
#include "player_strategy.hpp"
#include "random.hpp"
#include "simulation.hpp"
#include "statistics.hpp"


void print_statistics() {
	using namespace monopoly;

	std::cout << "Avg rounds per game: " << statistics.avg_rounds_per_game() << "\n\n";

	std::cout << "Avg player ranks:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.avg_player_rank(player) << '\n';
	}
	std::cout << '\n';

	std::cout << "Avg final net worths:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.avg_final_net_worth(player) << '\n';
	}
	std::cout << '\n';

	std::cout << "Avg times sent to jail per turn:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ": "
			<< statistics.avg_times_sent_to_jail_per_turn(player) << '\n';
	}
	std::cout << '\n';

	std::cout << "Avg jail duration:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ": "
			<< statistics.avg_jail_duration(player) << '\n';
	}
	std::cout << '\n';

	std::cout << "Avg rent payments:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ":\n"
			<< "    +" << statistics.avg_rent_received_per_game(player) << "/game  \t"
			<< "    +" << statistics.avg_rent_received_per_turn(player) << "/turn  \t"
			<< "    +" << statistics.avg_rent_received_per_rent(player) << "/rent\n"
			<< "    -" << statistics.avg_rent_paid_per_game(player) << "/game  \t"
			<< "    -" << statistics.avg_rent_paid_per_turn(player) << "/turn  \t"
			<< "    -" << statistics.avg_rent_paid_per_rent(player) << "/rent\n";
	}
	std::cout << '\n';

	std::cout << "Avg cards drawn per turn:\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ": " << statistics.avg_cards_drawn_per_turn(player) << '\n';
	}
	std::cout << '\n';

	std::cout << "Avg cash award card amount:\n";
	std::cout << "  " << statistics.avg_cash_award_card_amount_per_cash_award_card() << "/cash_award_card\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ": "
			<< statistics.avg_cash_award_card_amount_per_game(player) << "/game\n";
	}
	std::cout << '\n';

	std::cout << "Avg cash fee card amount:\n";
	std::cout << "  " << statistics.avg_cash_fee_card_amount_per_cash_fee_card() << "/cash_fee_card\n";
	for (auto const player : players) {
		std::cout << "  Player " << player << ": "
			<< statistics.avg_cash_fee_card_amount_per_game(player) << "/game\n";
	}
	std::cout << '\n';

	{
		std::cout << "Board space frequencies:\n";
		auto const rel_freqs = statistics.board_space_relative_frequencies();
		auto const board_spaces = sorted_indices(rel_freqs);
		for (auto const space : board_spaces | std::views::reverse) {
			std::cout << "  " << rel_freqs[space] << ": " << board_position_name(space) << '\n';
		}
		std::cout << '\n';
	}

	{
		std::cout << "Board space frequency skew:\n";
		for (auto const player : players) {
			std::cout << "  Player " << player << ":\n";
			auto const skews = statistics.board_space_frequency_skew(player);
			auto const board_spaces = sorted_indices(skews, [](double s) { return std::abs(s); });
			for (auto const space : board_spaces | std::views::reverse | std::views::take(5)) {
				auto const skew = skews[space];
				auto const sign = skew >= 0 ? "+" : "";
				std::cout << "    " << sign << skew << ": " << board_position_name(space) << '\n';
			}
		}
		std::cout << '\n';
	}

	{
		std::cout << "Avg street purchase first round:\n";
		auto const avg_rounds = statistics.avg_property_first_purchase_round<street_t>();
		auto const street_indices = sorted_indices(avg_rounds);
		for (auto const street_idx : street_indices) {
			std::cout << "  " << street_names[street_idx] << ": " << avg_rounds[street_idx] << '\n';
		}
		std::cout << '\n';
	}

	std::cout << "Simulation speed:\n"
		<< "  " << statistics.avg_games_per_second() << " game/sec\n"
		<< "  " << statistics.avg_rounds_per_second() << " round/sec\n"
		<< "  " << statistics.avg_turns_per_second() << " turn/sec\n"
		<< "  " << 1 / statistics.avg_games_per_second() << " sec/game\n"
		<< "  " << 1 / statistics.avg_rounds_per_second() << " sec/round\n"
		<< "  " << 1 / statistics.avg_turns_per_second() << " sec/turn\n";
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
