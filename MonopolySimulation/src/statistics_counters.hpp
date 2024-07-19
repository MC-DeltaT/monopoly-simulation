#pragma once

#include <array>
#include <bit>
#include <cstddef>

#include "common_constants.hpp"
#include "property_constants.hpp"
#include "per_propertytype_data.hpp"


namespace monopoly {

	inline constexpr bool record_stats = true;


	template<typename T, std::size_t N>
	struct counter_array : public std::array<T, N> {
		using std::array<T, N>::array;

		counter_array& operator+=(counter_array const& other) {
			for (std::size_t i = 0; i < N; ++i) {
				(*this)[i] += other[i];
			}
			return *this;
		}
	};


	using int_count = unsigned long long;
	using float_count = double;

	template<typename T>
	using per_player_counter = counter_array<T, player_count>;

	using per_player_int_count = per_player_counter<int_count>;

	template<typename T>
	using per_property = per_propertytype_data<
		std::array<T, street_count>,
		std::array<T, railway_count>,
		std::array<T, utility_count>>;

	using per_property_int_count = per_propertytype_data<
		counter_array<int_count, street_count>,
		counter_array<int_count, railway_count>,
		counter_array<int_count, utility_count>>;


	template<unsigned long long Max> requires (Max >= 2)
	struct log2_histogram {
		static constexpr unsigned long long true_max = std::bit_ceil(Max);
		static constexpr unsigned bin_count = std::bit_width(true_max);
		static_assert(bin_count >= 2);

		counter_array<int_count, bin_count> bins;

		constexpr void add(unsigned long long value) noexcept {
			++bins[compute_bin(value)];
		}

		[[nodiscard]]
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
		[[nodiscard]]
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

		log2_histogram& operator+=(log2_histogram const& other) {
			bins += other.bins;
			return *this;
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
		log2_histogram<100> game_length_histogram;

		// Number of turns played, for each player.
		// This includes all turns (including in jail) when the player is not bankrupt.
		// Extra turns from rolling doubles count multiple times.
		per_player_int_count turns_played{};

		// Number of times passed Go and collected Go salary, for each player.
		per_player_int_count go_passes{};

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
		per_player_counter<counter_array<int_count, board_space_count + 1>> board_space_counts{};

		// Number of times each player is sent to jail.
		per_player_int_count sent_to_jail_count{};

		// Turns spent in jail, for each player.
		// If a player gets into jail and gets out on the next turn, that counts as 1 turn in jail.
		per_player_int_count turns_in_jail{};

		// Number of times each player paid the jail fee to leave jail.
		per_player_int_count jail_fee_paid_count{};

		// Number of cards drawn, for each player.
		per_player_int_count cards_drawn{};

		// Cash received as a result of drawing immediate cash award cards, for each player.
		// Does not include receiving cash from another player who drew the card.
		per_player_int_count cash_award_card_amount{};

		// Number of cash award cards drawn, for each player.
		per_player_int_count cash_award_cards_drawn{};

		// Cash received as a result of another player drawing per-player cash fee card.
		per_player_int_count per_player_cash_fee_card_receive_amount{};

		// Number of times a player received cash from another player drawing a per-player cash fee card.
		per_player_int_count per_player_cash_fee_card_receive_count{};

		// Cash paid as a result of another player drawing a per-player cash award card.
		per_player_int_count per_player_cash_award_card_payment_amount{};

		// Number of times a player paid cash to another player drawing a per-player cash award card.
		per_player_int_count per_player_cash_award_card_payment_count{};

		// Cash paid as a result of drawing immediate cash fee cards, for each player.
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

		// Number of times a player won an unowned property auction.
		per_player_int_count unowned_property_auctions_won{};

		// Cash paid to purchase properties.
		per_player_int_count property_purchase_costs{};

		// Cash received from selling properties.
		per_player_int_count property_sell_income{};

		stat_counters_t& operator+=(stat_counters_t const& other) {
			auto const apply = [this, &other](auto const member) {
				this->*member += (&other)->*member;
			};

			// TODO: there must be an easier way to do this
			using T = stat_counters_t;
			apply(&T::simulation_time_seconds);
			apply(&T::games);
			apply(&T::rounds);
			apply(&T::game_length_histogram);
			apply(&T::turns_played);
			apply(&T::go_passes);
			apply(&T::player_rank);
			apply(&T::final_net_worth);
			apply(&T::rent_paid_amount);
			apply(&T::rent_paid_count);
			apply(&T::rent_received_amount);
			apply(&T::rent_received_count);
			apply(&T::board_space_counts);
			apply(&T::sent_to_jail_count);
			apply(&T::turns_in_jail);
			apply(&T::jail_fee_paid_count);
			apply(&T::cards_drawn);
			apply(&T::cash_award_card_amount);
			apply(&T::cash_award_cards_drawn);
			apply(&T::per_player_cash_fee_card_receive_amount);
			apply(&T::per_player_cash_fee_card_receive_count);
			apply(&T::per_player_cash_award_card_payment_amount);
			apply(&T::per_player_cash_award_card_payment_count);
			apply(&T::cash_fee_card_amount);
			apply(&T::cash_fee_cards_drawn);
			apply(&T::property_purchased_at_least_once);
			apply(&T::property_first_purchase_round);
			apply(&T::property_unowned_auction_price);
			apply(&T::property_unowned_auction_count);
			apply(&T::unowned_property_auctions_won);
			apply(&T::property_purchase_costs);
			apply(&T::property_sell_income);

			return *this;
		}
	};

	inline stat_counters_t operator+(stat_counters_t lhs, stat_counters_t const& rhs) {
		lhs += rhs;
		return lhs;
	}

	thread_local inline stat_counters_t stat_counters{};


	// Per-game state needed for tracking statistics.
	// Reset at the start of each game.
	struct stat_helper_state_t {
		// Indicates if each property has been purchased from the bank yet. Once true, remains true.
		per_property<bool> property_has_been_purchased{};
	};

	thread_local inline stat_helper_state_t stat_helper_state{};

}
