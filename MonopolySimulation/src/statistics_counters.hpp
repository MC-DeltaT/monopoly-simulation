#pragma once

#include <array>

#include "common_constants.hpp"


namespace monopoly {

	using int_count = unsigned long long;
	using float_count = double;
	using per_player_counter = std::array<int_count, player_count>;


	struct stat_counters_t {
		// Statistics are not guaranteed to be updated until the end of each game.

		// Real time elapsed simulating games contributing to these statistics.
		float_count simulation_time_seconds = 0;

		// Number of complete games contributing to these statistics.
		int_count game_count = 0;

		// Total number of game rounds.
		int_count round_count = 0;

		// Number of turns played, for each player.
		// This includes all turns (including in jail) when the player is not bankrupt.
		per_player_counter turn_count{};

		// Sum of end game ranks for each player.
		// 0 = first place, to player_count-1 = last place
		per_player_counter player_rank_sum{};

		// Sum of end game net worths for each player.
		per_player_counter final_net_worth_sum{};

		// Property rent paid, for each player.
		per_player_counter rent_paid_total{};

		// Number of times rent was paid, for each player.
		per_player_counter rent_paid_count{};

		// Property rent received, for each player.
		per_player_counter rent_received_total{};

		// Number of times rent was received, for each player.
		per_player_counter rent_received_count{};

		// Count of every time a player is on a space during their turn. Last entry is In Jail.
		std::array<int_count, board_space_count + 1> board_space_counts{};

		// Number of times any player touches any space.
		int_count position_count = 0;

		// Number of times each player is sent to jail.
		per_player_counter sent_to_jail_count{};

		// Total turns spent by any player in jail.
		// If a player gets into jail and gets out on the next turn, that counts as 1 turn in jail.
		int_count jail_duration_total = 0;

		// Number of cards drawn, for each player.
		per_player_counter cards_drawn{};

		// Cash received as a result of immediate cash award cards, for each player.
		per_player_counter card_cash_award_total{};

		// Number of cash award cards drawn, for each player.
		per_player_counter card_cash_award_count{};

		// Cash paid as a result of immediate cash fee cards, for each player.
		per_player_counter card_cash_fee_total{};

		// Number of cash fee cards drawn, for each player.
		per_player_counter card_cash_fee_count{};
	};

	inline stat_counters_t stat_counters{};

}
