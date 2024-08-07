#pragma once

#include <array>

#include "board_space_constants.hpp"
#include "common_constants.hpp"
#include "common_types.hpp"
#include "gameplay_constants.hpp"
#include "math.hpp"
#include "property_values.hpp"
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
		double avg_go_passes_per_turn(unsigned const player) const {
			return div(c->go_passes[player], c->turns_played[player]);
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
		double avg_jail_fee_per_game_approx(unsigned const player) const {
			// Approximate, true amount paid not tracked.
			return div(c->jail_fee_paid_count[player], c->games) * jail_release_cost;
		}

		[[nodiscard]]
		double avg_go_salary_per_game(unsigned const player) const {
			return div(c->go_passes[player], c->games) * go_salary;
		}

		[[nodiscard]]
		double avg_tax_space_paid_per_game_approx(unsigned const player) const {
			// Approximate, true amount paid not tracked.
			auto const income_tax_count =
				c->board_space_counts[player][static_cast<unsigned>(board_space_t::income_tax)];
			auto const income_tax_paid = div(income_tax_count, c->games) * income_tax;
			auto const super_tax_count =
				c->board_space_counts[player][static_cast<unsigned>(board_space_t::super_tax)];
			auto const super_tax_paid = div(super_tax_count, c->games) * super_tax;
			return income_tax_paid + super_tax_paid;
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

		[[nodiscard]]
		double avg_per_player_cash_fee_card_amount_received_per_game(unsigned const player) const {
			return div(c->per_player_cash_fee_card_receive_amount[player], c->games);
		}

		[[nodiscard]]
		double avg_per_player_cash_award_card_amount_paid_per_game(unsigned const player) const {
			return div(c->per_player_cash_award_card_payment_amount[player], c->games);
		}

		[[nodiscard]]
		double avg_property_purchase_costs_per_game(unsigned const player) const {
			return div(c->property_purchase_costs[player], c->games);
		}

		[[nodiscard]]
		double avg_property_sell_income_per_game(unsigned const player) const {
			return div(c->property_sell_income[player], c->games);
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

		template<PropertyType P>
		[[nodiscard]]
		auto avg_unowned_property_auction_price() const {
			auto const sum = c->property_unowned_auction_price.get<P>();
			std::array<double, sum.size()> result{};
			for (std::size_t i = 0; i < result.size(); ++i) {
				result[i] = div(sum[i], c->property_unowned_auction_count.get<P>()[i]);
			}
			return result;
		}

		template<PropertyType P>
		[[nodiscard]]
		auto avg_unowned_property_auction_premium() const {
			auto const auction_prices = avg_unowned_property_auction_price<P>();
			std::array<double, auction_prices.size()> result{};
			for (unsigned i = 0; i < auction_prices.size(); ++i) {
				auto const regular_cost = property_buy_cost(P{i});
				result[i] = (auction_prices[i] - regular_cost) / regular_cost;
			}
			return result;
		}

		[[nodiscard]]
		double avg_unowned_property_auctions_won_per_game(unsigned const player) const {
			return div(c->unowned_property_auctions_won[player], c->games);
		}

	private:
		stat_counters_t const* c;
	};

}
