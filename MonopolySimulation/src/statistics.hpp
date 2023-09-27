#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <ranges>
#include <type_traits>

#include "common_constants.hpp"


namespace monopoly {

	struct statistics_t;


	template<auto F, template<typename> typename... Bases>
	class statistic_t : public Bases<statistic_t<F, Bases...>>... {
	public:
		static constexpr bool is_per_player =
			std::ranges::random_access_range<std::invoke_result_t<decltype(F), statistics_t const&>>;

		constexpr statistic_t(statistics_t const& stats) noexcept :
			_stats{&stats}
		{}

		constexpr auto raw() const requires !is_per_player {
			return F(*_stats);
		}

		constexpr auto raw(unsigned const player) const requires is_per_player {
			return F(*_stats)[player];
		}

		// Public for access by mixins.
		statistics_t const* _stats;
	};


	template<class Derived>
	struct per_game_statistic {
		constexpr double per_game() requires (!Derived::is_per_player) {
			return static_cast<double>(static_cast<Derived const&>(*this).raw())
				/ static_cast<Derived const&>(*this)._stats->game_count;
		}

		constexpr double per_game(unsigned const player) requires (Derived::is_per_player) {
			return static_cast<double>(static_cast<Derived const&>(*this).raw(player))
				/ static_cast<Derived const&>(*this)._stats->game_count;
		}
	};

	template<class Derived>
	struct per_card_draw_statistic {
		constexpr double per_card_draw(unsigned const player) const requires (Derived::is_per_player) {
			return static_cast<double>(static_cast<Derived const&>(*this).raw(player))
				/ static_cast<Derived const&>(*this)._stats->cards_drawn[player];
		}
	};

	template<class Derived>
	struct per_turn_statistic {
		constexpr double per_turn(unsigned const player) const requires (Derived::is_per_player) {
			return static_cast<double>(static_cast<Derived const&>(*this).raw(player))
				/ static_cast<Derived const&>(*this)._stats->turn_count[player];
		}
	};

	template<auto F>
	struct per_sample_statistic {
		template<class Derived>
		struct type {
			constexpr double per_sample() const requires (!Derived::is_per_player) {
				return static_cast<double>(static_cast<Derived const&>(*this).raw())
					/ F(*static_cast<Derived const&>(*this)._stats);
			}

			constexpr double per_sample(unsigned const player) const requires (Derived::is_per_player) {
				return static_cast<double>(static_cast<Derived const&>(*this).raw(player))
					/ F(*static_cast<Derived const&>(*this)._stats)[player];
			}
		};
	};


	using per_player_count = std::array<unsigned long long, player_count>;


	struct statistics_t {
		// Statistics are not guaranteed to be updated until the end of each game.

		// Number of complete games contributing to these statistics.
		unsigned long long game_count = 0;

		// Sum of total game rounds.
		unsigned long long round_count = 0;

		// Number of turns played, for each player.
		// This includes all turns (including in jail) when the player is not bankrupt.
		per_player_count turn_count{};

		// Sum of end game ranks for each player.
		// 0 = first place, to player_count-1 = last place
		per_player_count player_rank_sum{};

		// Sum of end game net worths for each player.
		per_player_count final_net_worth_sum{};

		// Property rent paid, for each player.
		per_player_count rent_paid_total{};

		// Number of times rent was paid, for each player.
		per_player_count rent_paid_count{};

		// Property rent received, for each player.
		per_player_count rent_received_total{};

		// Number of times rent was received, for each player.
		per_player_count rent_received_count{};

		// Count of every time a player is on a space during their turn. Last entry is In Jail.
		std::array<unsigned long long, board_space_count + 1> board_space_counts{};

		// Number of times any player touches any space.
		unsigned long long position_count = 0;

		// Number of times each player is sent to jail.
		per_player_count sent_to_jail_count{};

		// Total turns spent by any player in jail.
		// If a player gets into jail and gets out on the next turn, that counts as 1 turn in jail.
		unsigned long long jail_duration_total = 0;

		// Number of cards drawn, for each player.
		per_player_count cards_drawn{};

		// Cash received as a result of immediate cash award cards, for each player.
		per_player_count card_cash_award_total{};

		// Number of cash award cards drawn, for each player.
		per_player_count card_cash_award_count{};

		// Cash paid as a result of immediate cash fee cards, for each player.
		per_player_count card_cash_fee_total{};

		// Number of cash fee cards drawn, for each player.
		per_player_count card_cash_fee_count{};


		// Mean game length in rounds.
		constexpr double game_length_mean() const noexcept {
			return static_cast<double>(round_count) / game_count;
		}

		// Mean player rank over all games.
		constexpr double player_rank_mean(unsigned const player) const {
			return static_cast<double>(player_rank_sum[player]) / game_count;
		}

		// Mean end-game net worth of player per game.
		constexpr double final_net_worth_mean(unsigned const player) const {
			return static_cast<double>(final_net_worth_sum[player]) / game_count;
		}

		// Mean rent paid.
		constexpr auto rent_paid_mean() const noexcept {
			return statistic_t<
				[](statistics_t const& s) { return s.rent_paid_total; },
				per_game_statistic,
				per_turn_statistic,
				per_sample_statistic<[](statistics_t const& s) { return s.rent_paid_count; }>::type
			>{*this};
		}

		// Mean rent received.
		constexpr auto rent_received_mean() const noexcept {
			return statistic_t<
				[](statistics_t const& s) { return s.rent_received_total; },
				per_game_statistic,
				per_turn_statistic,
				per_sample_statistic<[](statistics_t const& s) { return s.rent_received_count; }>::type
			>{*this};
		}

		// Relative frequency of board spaces over all games.
		constexpr std::array<double, board_space_count + 1> board_space_relative_freq() const {
			std::array<double, board_space_count + 1> result{};
			for (unsigned space = 0; space < result.size(); ++space) {
				result[space] = static_cast<double>(board_space_counts[space]) / position_count;
			}
			return result;
		}

		// Means times player is sent to jail.
		constexpr auto sent_to_jail_count_mean() const noexcept {
			return statistic_t<
				[](statistics_t const& s) { return s.sent_to_jail_count; },
				per_game_statistic,
				per_turn_statistic
			>{*this};
		}

		// Total number of times any player was sent to jail.
		constexpr unsigned long long sent_to_jail_count_total() const noexcept {
			return std::accumulate(sent_to_jail_count.cbegin(), sent_to_jail_count.cend(), 0ull);
		}

		// Mean duration in turns of being in jail, across all times a player is sent to jail.
		constexpr double jail_duration_mean() const noexcept {
			return static_cast<double>(jail_duration_total) / sent_to_jail_count_total();
		}

		// Mean number of cards drawn.
		constexpr auto cards_drawn_mean() const noexcept {
			return statistic_t<
				[](statistics_t const& s) { return s.cards_drawn; },
				per_game_statistic,
				per_turn_statistic
			>{*this};
		}

		// Mean cash received from immediate cash award cards.
		constexpr auto card_cash_award_mean() const noexcept {
			return statistic_t<
				[](statistics_t const& s) { return s.card_cash_award_total; },
				per_game_statistic,
				per_turn_statistic,
				per_card_draw_statistic,
				per_sample_statistic<[](statistics_t const& s) { return s.card_cash_award_count; }>::type
			>{*this};
		}

		// Mean cash payed to immediate cash fee cards.
		constexpr  auto card_cash_fee_mean() const noexcept {
			return statistic_t<
				[](statistics_t const& s) { return s.card_cash_fee_total; },
				per_game_statistic,
				per_turn_statistic,
				per_card_draw_statistic,
				per_sample_statistic<[](statistics_t const& s) { return s.card_cash_fee_count; }>::type
			>{*this};
		}
	};

	inline statistics_t statistics;

}
