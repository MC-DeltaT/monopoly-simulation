#pragma once

#include <array>
#include <bit>

#include "common_constants.hpp"
#include "property_constants.hpp"
#include "per_propertytype_data.hpp"


namespace monopoly {

	inline constexpr bool record_stats = true;


	using int_count = unsigned long long;
	using float_count = double;

	template<typename T>
	using per_player = std::array<T, player_count>;

	using per_player_int_count = per_player<int_count>;

	template<typename T>
	using per_property = per_propertytype_data<
		std::array<T, street_count>,
		std::array<T, railway_count>,
		std::array<T, utility_count>>;

	using per_property_int_count = per_property<int_count>;


	template<unsigned long long Max> requires (Max >= 2)
	struct log2_histogram {
		static constexpr unsigned long long true_max = std::bit_ceil(Max);
		static constexpr unsigned bin_count = std::bit_width(true_max);
		static_assert(bin_count >= 2);

		std::array<int_count, bin_count> bins;

		constexpr void add(unsigned long long value) noexcept {
			++bins[compute_bin(value)];
		}

		static constexpr unsigned compute_bin(unsigned long long value) noexcept {
			if (value >= true_max) {
				return bin_count - 1;
			}
			unsigned bin = 0;
			unsigned long long bin_upper_bound = 2;
			while (true) {
				if (value < bin_upper_bound) {
					return bin;
				}
				++bin;
				bin_upper_bound *= 2;
			}
		}

		// Returns exclusive upper bound of each bin.
		// The final bin is not included, which spans from true_max to infinity.
		static constexpr std::array<unsigned long long, bin_count - 1> bin_bounds() noexcept {
			std::array<unsigned long long, bin_count - 1> result{};
			unsigned long long bin_upper_bound = 2;
			for (unsigned i = 0; i < bin_count - 1; ++i) {
				result[i] = bin_upper_bound;
				bin_upper_bound *= 2;
			}
			return result;
		}

		// Calls func with arguments of (bin_lower, bin_upper, bin_value).
		// bin_upper is 0 for the last bin, representing infinity.
		void enumerate_bins(auto func) const {
			auto const bounds = bin_bounds();
			unsigned long long lower_bound = 0;
			for (unsigned i = 0; i < bin_count - 1; ++i) {
				func(lower_bound, bounds[i], bins[i]);
				lower_bound = bounds[i];
			}
			func(lower_bound, 0ull, bins.back());
		}
	};


	struct stat_counters_t {
		// Statistics are not guaranteed to be updated until the end of each game.

		// Real time elapsed simulating games contributing to these statistics.
		float_count simulation_time_seconds{};

		// Number of complete games contributing to these statistics.
		int_count games{};

		// Total number of game rounds.
		int_count rounds{};

		// Histogram of game lengths.
		log2_histogram<1000> game_length_histogram;

		// Number of turns played, for each player.
		// This includes all turns (including in jail) when the player is not bankrupt.
		per_player_int_count turns_played{};

		// (Sum of) end game ranks for each player.
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
		per_player<std::array<int_count, board_space_count + 1>> board_space_counts{};

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

		// Number of games a property was purchased from the back at least once.
		per_property_int_count property_purchased_at_least_once{};
		
		// (Sum of) round a property is first purchased from the bank.
		per_property_int_count property_first_purchase_round{};

		// (Sum of) sale price of auction when landing on an unowned property.
		per_property_int_count property_unowned_auction_price{};

		// Number of times a property was sold in auction when landing on it.
		per_property_int_count property_unowned_auction_count{};
	};

	inline stat_counters_t stat_counters{};


	// Per-game state needed for tracking statistics.
	// Reset st the start of each game.
	struct stat_helper_state_t {
		// Indicates if each property has been purchased from the bank yet. Once true, remains true.
		per_property<bool> property_has_been_purchased{};
	};

	inline stat_helper_state_t stat_helper_state{};

}
