#pragma once

#include <algorithm>
#include <array>

#include "common_constants.hpp"


namespace monopoly {

	using per_player_count = std::array<unsigned long long, player_count>;

	using per_player_stat = std::array<double, player_count>;


	struct statistics_t {
		unsigned long long game_count = 0;

		// Sum of total game rounds.
		unsigned long long round_count = 0;

		// Sum of end game ranks for each player.
		// 0 = first place, to player_count-1 = last place
		per_player_count player_rank_sum{};

		// Sum of end game net worths for each player.
		per_player_count final_net_worth_sum{};

		// Total property rent paid, for each player.
		per_player_count rent_paid_total{};

		// Total property rent received, for each player.
		per_player_count rent_received_total{};

		// Count of every time a player is on a space during their turn. Space 40 is In Jail.
		std::array<unsigned long long, 41> board_space_counts{};

		// Number of times any player touches any space.
		unsigned long long position_count = 0;

		// Number of times each player is sent to jail.
		per_player_count sent_to_jail_count{};

		// Total turns spent by any player in jail.
		// If a player gets into jail and gets out on the next turn, that counts as 1 turn in jail.
		unsigned long long jail_duration_total = 0;

		// Total cash received as a result of immediate cash award cards, for each player.
		per_player_count card_cash_award_total{};

		// Total number of cash award cards drawn, for each player.
		per_player_count card_cash_award_count{};

		// Total cash paid as a result of immediate cash fee cards, for each player.
		per_player_count card_cash_fee_total{};

		// Total number of cash fee cards drawn, for each player.
		per_player_count card_cash_fee_count{};


		// Mean game length in rounds.
		double game_length_mean() const {
			return static_cast<double>(round_count) / game_count;
		}

		// Mean player rank over all games.
		double player_rank_mean(unsigned const player) const {
			return static_cast<double>(player_rank_sum[player]) / game_count;
		}

		// Mean end-game net worth of player per game.
		double final_net_worth_mean(unsigned const player) const {
			return static_cast<double>(final_net_worth_sum[player]) / game_count;
		}

		// Mean rent paid by player per game.
		double rent_paid_mean_per_game(unsigned const player) const {
			return static_cast<double>(rent_paid_total[player]) / game_count;
		}

		// Mean rent received by player per game.
		double rent_received_mean_per_game(unsigned const player) const {
			return static_cast<double>(rent_received_total[player]) / game_count;
		}

		// Relative frequency of board space over all games.
		double board_space_relative_freq(unsigned const board_space) const {
			return static_cast<double>(board_space_counts[board_space]) / position_count;
		}

		// Mean times player is sent to jail per game.
		double sent_to_jail_count_mean_per_game(unsigned const player) const {
			return static_cast<double>(sent_to_jail_count[player]) / game_count;
		}

		// Total number of times any player was sent to jail.
		unsigned long long sent_to_jail_count_total() const {
			return std::accumulate(sent_to_jail_count.cbegin(), sent_to_jail_count.cend(), 0ull);
		}

		// Mean duration in turns of being in jail, across all times a player is sent to jail.
		double jail_duration_mean() const {
			return static_cast<double>(jail_duration_total) / sent_to_jail_count_total();
		}

		// Mean cash received from immediate cash award cards per game.
		double card_cash_award_mean_per_game(unsigned const player) const {
			return static_cast<double>(card_cash_award_total[player]) / card_cash_award_count[player];
		}

		// Mean cash payed to immediate cash fee cards per game.
		double card_cash_fee_mean_per_game(unsigned const player) const {
			return static_cast<double>(card_cash_fee_total[player]) / card_cash_fee_count[player];
		}
	};

	inline statistics_t statistics;

}
