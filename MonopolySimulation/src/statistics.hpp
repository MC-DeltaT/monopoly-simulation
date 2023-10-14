#pragma once

#include <algorithm>
#include <array>
#include <ranges>

#include "common_constants.hpp"
#include "common_types.hpp"
#include "statistics_counters.hpp"


namespace monopoly {

	class statistics_t {
	public:
		explicit constexpr statistics_t(stat_counters_t const& counters) noexcept :
			c{&counters}
		{}

		[[nodiscard]]
		double avg_games_per_second() const {
			return div(c->games, c->simulation_time_seconds);
		}

		[[nodiscard]]
		double avg_rounds_per_second() const {
			return div(c->rounds, c->simulation_time_seconds);
		}
		
		[[nodiscard]]
		double avg_turns_per_second() const {
			return div(sum(c->turns_played), c->simulation_time_seconds);
		}
		
		[[nodiscard]]
		double avg_rounds_per_game() const {
			return div(c->rounds, c->games);
		}
		
		[[nodiscard]]
		double avg_player_rank(unsigned const player) const {
			return div(c->player_rank[player], c->games);
		}
		
		[[nodiscard]]
		double avg_final_net_worth(unsigned const player) const {
			return div(c->final_net_worth[player], c->games);
		}
		
		[[nodiscard]]
		double avg_turns_per_game(unsigned const player) const {
			return div(c->turns_played[player] - 1, c->games);
		}
		
		[[nodiscard]]
		std::array<double, board_space_count + 1> board_space_relative_frequencies(unsigned const player) const {
			auto const total = sum(c->board_space_counts[player]);
			std::array<double, board_space_count + 1> result{};
			for (unsigned space = 0; space < result.size(); ++space) {
				result[space] = div(c->board_space_counts[player][space], total);
			}
			return result;
		}

		[[nodiscard]]
		std::array<double, board_space_count + 1> board_space_relative_frequencies() const {
			std::array<double, board_space_count + 1> overall_freqs{};
			for (auto const& player_counts : c->board_space_counts) {
				for (unsigned i = 0; i < board_space_count + 1; ++i) {
					overall_freqs[i] += player_counts[i];
				}
			}
			auto const total = sum(overall_freqs);
			for (auto& count : overall_freqs) {
				count /= total;
			}
			return overall_freqs;
		}

		[[nodiscard]]
		std::array<double, board_space_count + 1> board_space_frequency_skew(unsigned const player) const {
			auto const overall_freqs = board_space_relative_frequencies();
			auto const player_total = sum(c->board_space_counts[player]);
			std::array<double, board_space_count + 1> skews{};
			for (unsigned i = 0; i < board_space_count + 1; ++i) {
				skews[i] = div(c->board_space_counts[player][i], player_total) - overall_freqs[i];
			}
			return skews;
		}
		
		[[nodiscard]]
		double avg_times_sent_to_jail_per_turn(unsigned const player) const {
			return div(c->sent_to_jail_count[player], c->turns_played[player]);
		}
		
		[[nodiscard]]
		double avg_jail_duration(unsigned const player) const {
			return div(c->turns_in_jail[player], c->sent_to_jail_count[player]);
		}
		
		[[nodiscard]]
		double avg_rent_paid_per_game(unsigned const player) const {
			return div(c->rent_paid_amount[player], c->games);
		}
		
		[[nodiscard]]
		double avg_rent_paid_per_turn(unsigned const player) const {
			return div(c->rent_paid_amount[player], c->turns_played[player]);
		}
		
		[[nodiscard]]
		double avg_rent_paid_per_rent(unsigned const player) const {
			return div(c->rent_paid_amount[player], c->rent_paid_count[player]);
		}
		
		[[nodiscard]]
		double avg_rent_received_per_game(unsigned const player) const {
			return div(c->rent_received_amount[player], c->games);
		}
		
		[[nodiscard]]
		double avg_rent_received_per_turn(unsigned const player) const {
			return div(c->rent_received_amount[player], c->turns_played[player]);
		}
		
		[[nodiscard]]
		double avg_rent_received_per_rent(unsigned const player) const {
			return div(c->rent_received_amount[player], c->rent_received_count[player]);
		}
		
		[[nodiscard]]
		double avg_cards_drawn_per_turn(unsigned const player) const {
			return div(c->cards_drawn[player], c->turns_played[player]);
		}
		
		[[nodiscard]]
		double avg_cash_award_card_amount_per_cash_award_card() const {
			return div(sum(c->cash_award_card_amount), sum(c->cash_award_cards_drawn));
		}
		
		[[nodiscard]]
		double avg_cash_award_card_amount_per_game(unsigned const player) const {
			return div(c->cash_award_card_amount[player], c->games);
		}
		
		[[nodiscard]]
		double avg_cash_fee_card_amount_per_cash_fee_card() const {
			return div(sum(c->cash_fee_card_amount), sum(c->cash_fee_cards_drawn));
		}
		
		[[nodiscard]]
		double avg_cash_fee_card_amount_per_game(unsigned const player) const {
			return div(c->cash_fee_card_amount[player], c->games);
		}

		template<PropertyType P>
		[[nodiscard]]
		auto avg_property_first_purchase_round() const {
			auto const sum = c->property_first_purchase_round.get<P>();
			std::array<double, sum.size()> result{};
			for (std::size_t i = 0; i < result.size(); ++i) {
				result[i] = div(sum[i], c->property_purchased_at_least_once.get<P>()[i]);
			}
			return result;
		}

		[[nodiscard]]
		double avg_unowned_property_auctions_won_per_game(unsigned const player) const {
			return div(c->unowned_property_auctions_won[player], c->games);
		}

	private:
		stat_counters_t const* c;
		
		template<typename T1, typename T2>
		[[nodiscard]]
		static double div(T1 const lhs, T2 const rhs) noexcept {
			return static_cast<double>(lhs) / static_cast<double>(rhs);
		}
		
		template<std::ranges::range R>
		[[nodiscard]]
		static std::ranges::range_value_t<R> sum(R const& r) {
			return std::accumulate(std::ranges::begin(r), std::ranges::end(r), std::ranges::range_value_t<R>{});
		}
	};

	inline statistics_t statistics{stat_counters};

}
