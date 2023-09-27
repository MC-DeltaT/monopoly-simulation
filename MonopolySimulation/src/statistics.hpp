#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <ranges>
#include <type_traits>

#include "common_constants.hpp"
#include "statistics_counters.hpp"


template<typename T1, typename T2> requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
constexpr double true_div(T1 const lhs, T2 const rhs) noexcept {
	return static_cast<double>(lhs) / static_cast<double>(rhs);
}


namespace monopoly {

	// TODO: needs to be more simple and flexible. some weirdness: per turn vs. per player turn vs. per round ??

	template<auto F, template<typename> typename... Bases>
	class statistic_t : public Bases<statistic_t<F, Bases...>>... {
	public:
		static constexpr bool is_per_player =
			std::ranges::random_access_range<std::invoke_result_t<decltype(F), stat_counters_t const&>>;

		constexpr statistic_t(stat_counters_t const& counters) noexcept :
			_counters{&counters}
		{}

		constexpr auto raw() const requires (!is_per_player) {
			return F(*_counters);
		}

		constexpr auto raw(unsigned const player) const requires is_per_player {
			return F(*_counters)[player];
		}

		// Public for access by mixins.
		stat_counters_t const* _counters;
	};


	template<class Derived>
	struct per_game_statistic {
		constexpr double per_game() requires (!Derived::is_per_player) {
			return true_div(static_cast<Derived const&>(*this).raw(),
				static_cast<Derived const&>(*this)._counters->game_count);
		}

		constexpr double per_game(unsigned const player) requires (Derived::is_per_player) {
			return true_div(static_cast<Derived const&>(*this).raw(player),
				static_cast<Derived const&>(*this)._counters->game_count);
		}
	};

	template<class Derived>
	struct per_round_statistic {
		constexpr double per_round() const requires (!Derived::is_per_player) {
			return true_div(static_cast<Derived const&>(*this).raw(),
				static_cast<Derived const&>(*this)._counters->round_count);
		}
	};

	template<class Derived>
	struct per_turn_statistic {
		// Per total number of turns across all players.
		constexpr double per_turn() const requires (!Derived::is_per_player) {
			auto const& count = static_cast<Derived const&>(*this)._counters->turn_count;
			auto const total_turns = std::accumulate(count.cbegin(), count.cend(), 0ull);

			return true_div(static_cast<Derived const&>(*this).raw(), total_turns);
		}

		// Per turns made by one specific player.
		constexpr double per_turn(unsigned const player) const requires (Derived::is_per_player) {
			return true_div(static_cast<Derived const&>(*this).raw(player),
				static_cast<Derived const&>(*this)._counters->turn_count[player]);
		}
	};

	template<class Derived>
	struct per_card_draw_statistic {
		// Per card draws made by one specific player.
		constexpr double per_card_draw(unsigned const player) const requires (Derived::is_per_player) {
			return true_div(static_cast<Derived const&>(*this).raw(player),
				static_cast<Derived const&>(*this)._counters->cards_drawn[player]);
		}
	};

	template<auto F>
	struct per_sample_statistic {
		template<class Derived>
		struct type {
			constexpr double per_sample() const requires (!Derived::is_per_player) {
				return true_div(static_cast<Derived const&>(*this).raw(),
					F(*static_cast<Derived const&>(*this)._counters));
			}

			constexpr double per_sample(unsigned const player) const requires (Derived::is_per_player) {
				return true_div(static_cast<Derived const&>(*this).raw(player),
					F(*static_cast<Derived const&>(*this)._counters)[player]);
			}
		};
	};


	class statistics_t {
	public:
		constexpr statistics_t(stat_counters_t const& counters) noexcept :
			simulation_time_mean{counters},
			rounds_mean{counters},
			rent_paid_mean{counters},
			rent_received_mean{counters},
			sent_to_jail_count_mean{counters},
			cards_drawn_mean{counters},
			card_cash_award_mean{counters},
			card_cash_fee_mean{counters},
			_counters{&counters}
		{}

		constexpr int_count total_player_turns() const noexcept {
			auto const& count = _counters->turn_count;
			return std::accumulate(count.cbegin(), count.cend(), 0ull);
		}

		// Mean real time taken during simulation.
		statistic_t<
			[](stat_counters_t const& c) { return c.simulation_time_seconds; },
			per_game_statistic,
			per_round_statistic,
			per_turn_statistic
		> simulation_time_mean;

		statistic_t<
			[](stat_counters_t const& c) { return c.round_count; },
			per_game_statistic
		> rounds_mean;

		// Mean player rank over all games.
		constexpr double player_rank_mean(unsigned const player) const {
			return true_div(_counters->player_rank_sum[player], _counters->game_count);
		}

		// Mean end-game net worth of player per game.
		constexpr double final_net_worth_mean(unsigned const player) const {
			return true_div(_counters->final_net_worth_sum[player], _counters->game_count);
		}

		// Mean rent paid.
		statistic_t<
			[](stat_counters_t const& c) { return c.rent_paid_total; },
			per_game_statistic,
			per_turn_statistic,
			per_sample_statistic<[](stat_counters_t const& s) { return s.rent_paid_count; }>::type
		> rent_paid_mean;

		// Mean rent received.
		statistic_t<
			[](stat_counters_t const& c) { return c.rent_received_total; },
			per_game_statistic,
			per_turn_statistic,
			per_sample_statistic<[](stat_counters_t const& s) { return s.rent_received_count; }>::type
		> rent_received_mean;

		// Relative frequency of board spaces over all games.
		constexpr std::array<double, board_space_count + 1> board_space_relative_freq() const {
			std::array<double, board_space_count + 1> result{};
			for (unsigned space = 0; space < result.size(); ++space) {
				result[space] = true_div(_counters->board_space_counts[space], _counters->position_count);
			}
			return result;
		}

		// Means times player is sent to jail.
		statistic_t<
			[](stat_counters_t const& s) { return s.sent_to_jail_count; },
			per_game_statistic,
			per_turn_statistic
		> sent_to_jail_count_mean;

		// Total number of times any player was sent to jail.
		constexpr int_count sent_to_jail_count_total() const noexcept {
			auto const& count = _counters->sent_to_jail_count;
			return std::accumulate(count.cbegin(), count.cend(), 0ull);
		}

		// Mean duration in turns of being in jail, across all times a player is sent to jail.
		constexpr double jail_duration_mean() const noexcept {
			return true_div(_counters->jail_duration_total, sent_to_jail_count_total());
		}

		// Mean number of cards drawn.
		statistic_t<
			[](stat_counters_t const& c) { return c.cards_drawn; },
			per_game_statistic,
			per_turn_statistic
		> cards_drawn_mean;

		// Mean cash received from immediate cash award cards.
		statistic_t<
			[](stat_counters_t const& c) { return c.card_cash_award_total; },
			per_game_statistic,
			per_turn_statistic,
			per_card_draw_statistic,
			per_sample_statistic<[](stat_counters_t const& c) { return c.card_cash_award_count; }>::type
		> card_cash_award_mean;

		// Mean cash payed to immediate cash fee cards.
		statistic_t<
			[](stat_counters_t const& c) { return c.card_cash_fee_total; },
			per_game_statistic,
			per_turn_statistic,
			per_card_draw_statistic,
			per_sample_statistic<[](stat_counters_t const& c) { return c.card_cash_fee_count; }>::type
		> card_cash_fee_mean;

	private:
		stat_counters_t const* _counters;
	};

	inline statistics_t statistics{stat_counters};

}
