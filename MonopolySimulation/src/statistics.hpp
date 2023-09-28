#pragma once

#include <algorithm>
#include <array>
#include <ranges>
#include <type_traits>

#include "common_constants.hpp"
#include "statistics_counters.hpp"


namespace monopoly {

	class statistics_t {
	public:
		constexpr statistics_t(stat_counters_t const& counters) noexcept :
			c{&counters}
		{}

		double avg_games_per_second() const {
			return div(c->games, c->simulation_time_seconds);
		}

		double avg_rounds_per_second() const {
			return div(c->rounds, c->simulation_time_seconds);
		}

		double avg_turns_per_second() const {
			return div(sum(c->turns_played), c->simulation_time_seconds);
		}

		double avg_rounds_per_game() const {
			return div(c->rounds, c->games);
		}

		double avg_player_rank(unsigned const player) const {
			return div(c->player_rank[player], c->games);
		}

		double avg_final_net_worth(unsigned const player) const {
			return div(c->final_net_worth[player], c->games);
		}

		double avg_turns_per_game(unsigned const player) const {
			return div(c->turns_played[player] - 1, c->games);
		}

		std::array<double, board_space_count + 1> board_space_relative_frequencies() const {
			auto const total = sum(c->board_space_counts);
			std::array<double, board_space_count + 1> result{};
			for (unsigned space = 0; space < result.size(); ++space) {
				result[space] = div(c->board_space_counts[space], total);
			}
			return result;
		}

		double avg_times_sent_to_jail_per_turn(unsigned const player) const {
			return div(c->sent_to_jail_count[player], c->turns_played[player]);
		}

		double avg_jail_duration(unsigned const player) const {
			return div(c->turns_in_jail[player], c->sent_to_jail_count[player]);
		}

		double avg_rent_paid_per_game(unsigned const player) const {
			return div(c->rent_paid_amount[player], c->games);
		}

		double avg_rent_paid_per_turn(unsigned const player) const {
			return div(c->rent_paid_amount[player], c->turns_played[player]);
		}

		double avg_rent_paid_per_rent(unsigned const player) const {
			return div(c->rent_paid_amount[player], c->rent_paid_count[player]);
		}

		double avg_rent_received_per_game(unsigned const player) const {
			return div(c->rent_received_amount[player], c->games);
		}

		double avg_rent_received_per_turn(unsigned const player) const {
			return div(c->rent_received_amount[player], c->turns_played[player]);
		}

		double avg_rent_received_per_rent(unsigned const player) const {
			return div(c->rent_received_amount[player], c->rent_received_count[player]);
		}

		double avg_cards_drawn_per_turn(unsigned const player) const {
			return div(c->cards_drawn[player], c->turns_played[player]);
		}

		double avg_cash_award_card_amount_per_cash_award_card() const {
			return div(sum(c->cash_award_card_amount), sum(c->cash_award_cards_drawn));
		}

		double avg_cash_award_card_amount_per_game(unsigned const player) const {
			return div(c->cash_award_card_amount[player], c->games);
		}

		double avg_cash_fee_card_amount_per_cash_fee_card() const {
			return div(sum(c->cash_fee_card_amount), sum(c->cash_fee_cards_drawn));
		}

		double avg_cash_fee_card_amount_per_game(unsigned const player) const {
			return div(c->cash_fee_card_amount[player], c->games);
		}

	private:
		stat_counters_t const* c;

		template<typename T1, typename T2> requires std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>
		static constexpr double div(T1 const lhs, T2 const rhs) noexcept {
			return static_cast<double>(lhs) / static_cast<double>(rhs);
		}

		template<std::ranges::range R>
		static constexpr auto sum(R const& r) {
			return std::accumulate(std::ranges::begin(r), std::ranges::end(r), std::ranges::range_value_t<R>{});
		}
	};


	inline statistics_t statistics{stat_counters};

}
