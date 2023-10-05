#pragma once

#include <array>

#include "common_constants.hpp"


namespace monopoly {

	using int_count = unsigned long long;
	using float_count = double;
	using per_player_int_count = std::array<int_count, player_count>;


	struct stat_counters_t {
		// Statistics are not guaranteed to be updated until the end of each game.

		// Real time elapsed simulating games contributing to these statistics.
		float_count simulation_time_seconds{};

		// Number of complete games contributing to these statistics.
		int_count games{};

		// Total number of game rounds.
		int_count rounds{};

		// Number of turns played, for each player.
		// This includes all turns (including in jail) when the player is not bankrupt.
		per_player_int_count turns_played{};

		// Sum of end game ranks for each player.
		// 0 = first place, to player_count-1 = last place
		per_player_int_count player_rank{};

		// Sum of end game net worths for each player.
		per_player_int_count final_net_worth{};

		// Property rent paid, for each player.
		per_player_int_count rent_paid_amount{};

		// Number of times rent was paid, for each player.
		per_player_int_count rent_paid_count{};

		// Property rent received, for each player.
		per_player_int_count rent_received_amount{};

		// Number of times rent was received, for each player.
		per_player_int_count rent_received_count{};

		// Count of every time a player is on a space during their turn. Last entry is In Jail.
		std::array<std::array<int_count, board_space_count + 1>, player_count> board_space_counts{};

		// Number of times each player is sent to jail.
		per_player_int_count sent_to_jail_count{};

		// Turns spent in jail, for each player.
		// If a player gets into jail and gets out on the next turn, that counts as 1 turn in jail.
		per_player_int_count turns_in_jail{};

		// Number of cards drawn, for each player.
		per_player_int_count cards_drawn{};

		// Cash received as a result of immediate cash award cards, for each player.
		per_player_int_count cash_award_card_amount{};

		// Number of cash award cards drawn, for each player.
		per_player_int_count cash_award_cards_drawn{};

		// Cash paid as a result of immediate cash fee cards, for each player.
		per_player_int_count cash_fee_card_amount{};

		// Number of cash fee cards drawn, for each player.
		per_player_int_count cash_fee_cards_drawn{};
	};

	inline stat_counters_t stat_counters{};

}
