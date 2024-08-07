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
#include "statistics_counters.hpp"


namespace monopoly {

	void print_statistics(stat_counters_t const& stat_counters) {
		statistics_t const statistics{stat_counters};

		std::cout << "Games: " << stat_counters.games << "\n\n";

		std::cout << "Avg rounds per game: " << statistics.avg_rounds_per_game() << "\n\n";

		{
			std::cout << "Game length histogram:\n";
			stat_counters.game_length_histogram.enumerate_bins(
				[&stat_counters](auto const lower, auto const upper, auto const value) {
					auto const freq = static_cast<double>(value) / stat_counters.games;
					std::cout << "  " << lower << '-';
					if (upper > 0) {
						std::cout << upper;
					}
					else {
						std::cout << "inf";
					}
					std::cout << ": " << freq << '\n';
				});
			std::cout << '\n';
		}

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

		std::cout << "Avg cash income per game breakdown:\n";
		for (auto const player : players) {
			std::cout << "  Player " << player << ":\n";
			std::cout << "    " << statistics.avg_go_salary_per_game(player) << " Go salary\n";
			std::cout << "    " << statistics.avg_rent_received_per_game(player) << " rent\n";
			std::cout << "    " << statistics.avg_property_sell_income_per_game(player) << " property sale\n";
			std::cout << "    " << statistics.avg_cash_award_card_amount_per_game(player) << " cash award card\n";
			std::cout << "    " << statistics.avg_per_player_cash_fee_card_amount_received_per_game(player)
				<< " per-player cash fee card\n";
		}
		std::cout << '\n';

		std::cout << "Avg cash expenditure per game breakdown:\n";
		for (auto const player : players) {
			std::cout << "  Player " << player << ":\n";
			std::cout << "    " << statistics.avg_tax_space_paid_per_game_approx(player) << " tax space\n";
			std::cout << "    " << statistics.avg_jail_fee_per_game_approx(player) << " jail fee\n";
			std::cout << "    " << statistics.avg_rent_paid_per_game(player) << " rent\n";
			std::cout << "    " << statistics.avg_property_purchase_costs_per_game(player) << " property purchase\n";
			std::cout << "    " << statistics.avg_cash_fee_card_amount_per_game(player) << " cash fee card\n";
			std::cout << "    " << statistics.avg_per_player_cash_award_card_amount_paid_per_game(player)
				<< " per-player cash award card\n";
		}
		std::cout << '\n';

		std::cout << "Avg times passed Go per turn:\n";
		for (auto const player : players) {
			std::cout << "  Player " << player << ": " << statistics.avg_go_passes_per_turn(player) << '\n';
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
				<< "    +" << statistics.avg_rent_received_per_turn(player) << "/turn  \t"
				<< "    +" << statistics.avg_rent_received_per_rent(player) << "/rent\n"
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
		std::cout << '\n';

		std::cout << "Avg cash fee card amount:\n";
		std::cout << "  " << statistics.avg_cash_fee_card_amount_per_cash_fee_card() << "/cash_fee_card\n";
		std::cout << '\n';
	
		std::cout << "Avg unowned property auctions won:\n";
		for (auto const player : players) {
			std::cout << "  Player " << player << ": "
				<< statistics.avg_unowned_property_auctions_won_per_game(player) << "/game\n";
		}
		std::cout << '\n';

		{
			std::cout << "Board space relative frequencies:\n";
			auto const rel_freqs = statistics.board_space_relative_frequencies();
			auto const board_spaces = sorted_indices(rel_freqs);
			for (auto const space : board_spaces | std::views::reverse) {
				std::cout << "  " << rel_freqs[space] << ": " << board_position_name(space) << '\n';
			}
			std::cout << '\n';
		}

		{
			std::cout << "Board space frequency skew (absolute):\n";
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

		{
			std::cout << "Avg railway purchase first round:\n";
			auto const avg_rounds = statistics.avg_property_first_purchase_round<railway_t>();
			auto const railway_indices = sorted_indices(avg_rounds);
			for (auto const railway_idx : railway_indices) {
				std::cout << "  " << railway_names[railway_idx] << ": " << avg_rounds[railway_idx] << '\n';
			}
			std::cout << '\n';
		}

		{
			std::cout << "Avg utility purchase first round:\n";
			auto const avg_rounds = statistics.avg_property_first_purchase_round<utility_t>();
			auto const utility_indices = sorted_indices(avg_rounds);
			for (auto const utility_idx : utility_indices) {
				std::cout << "  " << utility_names[utility_idx] << ": " << avg_rounds[utility_idx] << '\n';
			}
			std::cout << '\n';
		}

		{
			std::cout << "Avg unowned street auction premium (proportional):\n";
			auto const premiums = statistics.avg_unowned_property_auction_premium<street_t>();
			auto const street_indices = sorted_indices(premiums, [](double s) { return std::abs(s); });
			for (auto const street : street_indices | std::views::reverse | std::views::take(5)) {
				auto const premium = premiums[street];
				auto const sign = premium >= 0 ? "+" : "";
				std::cout << "    " << sign << premium << ": " << street_names[street] << '\n';
			}
			std::cout << '\n';
		}

		{
			std::cout << "Avg unowned railway auction premium (proportional):\n";
			auto const premiums = statistics.avg_unowned_property_auction_premium<railway_t>();
			auto const railway_indices = sorted_indices(premiums, [](double s) { return std::abs(s); });
			for (auto const railway : railway_indices | std::views::reverse) {
				auto const premium = premiums[railway];
				auto const sign = premium >= 0 ? "+" : "";
				std::cout << "    " << sign << premium << ": " << railway_names[railway] << '\n';
			}
			std::cout << '\n';
		}

		{
			std::cout << "Avg unowned utility auction premium (proportional):\n";
			auto const premiums = statistics.avg_unowned_property_auction_premium<utility_t>();
			auto const utility_indices = sorted_indices(premiums, [](double s) { return std::abs(s); });
			for (auto const utility : utility_indices | std::views::reverse) {
				auto const premium = premiums[utility];
				auto const sign = premium >= 0 ? "+" : "";
				std::cout << "    " << sign << premium << ": " << utility_names[utility] << '\n';
			}
			std::cout << '\n';
		}

		// TODO: may want to show real time, not CPU time, in the case of multithreading
		std::cout << "Simulation speed:\n"
			<< "  " << statistics.avg_games_per_second() << " game/CPUsec\n"
			<< "  " << statistics.avg_rounds_per_second() << " round/CPUsec\n"
			<< "  " << statistics.avg_turns_per_second() << " turn/CPUsec\n"
			<< "  " << 1 / statistics.avg_games_per_second() << " CPUsec/game\n"
			<< "  " << 1 / statistics.avg_rounds_per_second() << " CPUsec/round\n"
			<< "  " << 1 / statistics.avg_turns_per_second() << " CPUsec/turn\n";
	}

}


int main() {
	using namespace monopoly;

#if defined(NDEBUG) || defined(RELEASE)
	constexpr std::size_t game_count = 1'000'000;
#else
	constexpr std::size_t game_count = 1000;
#endif
	constexpr auto max_rounds = 100;

	auto const strategies_factory = [] {
		return player_strategies_t{};
	};

	auto const random_factory = [] {
		return random_t{std::random_device{}()};
	};

	run_simulations_multithreaded(strategies_factory, random_factory, game_count, max_rounds);

	if (record_stats) {
		print_statistics(stat_counters);
	}
}
