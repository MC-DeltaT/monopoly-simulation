#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <ranges>

#include "common_constants.hpp"


namespace monopoly {

	struct statistics_t;


	template<typename F1, typename F2, template<typename> typename... Bases>
	class mean_stat_t : public Bases<mean_stat_t<F1, F2, Bases...>>... {
	public:
		static constexpr bool is_per_player =
			std::ranges::random_access_range<std::invoke_result_t<F1, statistics_t const&>>;
		static constexpr bool has_per_sample = !std::same_as<F2, std::nullptr_t>;
		static constexpr bool has_per_turn = is_per_player;

		mean_stat_t(statistics_t const& stats, F1 raw, F2 sample_count) :
			_statistics{&stats},
			_raw{raw},
			_sample_count{sample_count}
		{}

		auto raw() const requires !is_per_player {
			return _raw(*_statistics);
		}

		auto raw(unsigned const player) const requires is_per_player {
			return _raw(*_statistics)[player];
		}

		// TODO: everything a mixin

		double per_game() const requires !is_per_player {
			return static_cast<double>(raw()) / _statistics->game_count;
		}

		double per_game(unsigned const player) const requires is_per_player {
			return static_cast<double>(raw(player)) / _statistics->game_count;
		}

		double per_turn(unsigned const player) const requires has_per_turn {
			return static_cast<double>(raw(player)) / _statistics->turn_count[player];
		}

		double per_sample() const requires (has_per_sample && !is_per_player) {
			return static_cast<double>(raw()) / sample_count();
		}

		double per_sample(unsigned const player) const requires (has_per_sample && is_per_player) {
			return static_cast<double>(raw(player)) / sample_count(player);
		}

		unsigned long long sample_count() const requires (has_per_sample && !is_per_player) {
			return _sample_count(*_statistics);
		}

		unsigned long long sample_count(unsigned const player) const requires (has_per_sample && is_per_player) {
			return _sample_count(*_statistics)[player];
		}

		// Public for access by mixins.
		statistics_t const* _statistics;
		F1 _raw;
		F2 _sample_count;
	};

	template<template<typename> typename... Bases>
	auto mean_stat(statistics_t const& stats, auto raw, auto sample_count) {
		return mean_stat_t<decltype(raw), decltype(sample_count), Bases...>{
			stats, raw, sample_count};
	}

	template<template<typename> typename... Bases>
	auto mean_stat(statistics_t const& stats, auto raw) {
		return mean_stat_t<decltype(raw), std::nullptr_t, Bases...>{
			stats, raw, nullptr};
	}


	template<typename Derived>
	struct per_draw_mean_stat_mixin {
		double per_draw(unsigned const player) const requires (Derived::is_per_player) {
			return static_cast<double>(static_cast<Derived const&>(*this).raw(player))
				/ static_cast<Derived const&>(*this)._statistics->cards_drawn[player];
		}
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

		// Total property rent paid, for each player.
		per_player_count rent_paid_total{};

		// Number of times rent was paid, for each player.
		per_player_count rent_paid_count{};

		// Total property rent received, for each player.
		per_player_count rent_received_total{};

		// Number of times rent was received, for each player.
		per_player_count rent_received_count{};

		// Count of every time a player is on a space during their turn. Space 40 is In Jail.
		std::array<unsigned long long, 41> board_space_counts{};

		// Number of times any player touches any space.
		unsigned long long position_count = 0;

		// Number of times each player is sent to jail.
		per_player_count sent_to_jail_count{};

		// Total turns spent by any player in jail.
		// If a player gets into jail and gets out on the next turn, that counts as 1 turn in jail.
		unsigned long long jail_duration_total = 0;

		// Number of cards drawn, for each player.
		per_player_count cards_drawn{};

		// Total cash received as a result of immediate cash award cards, for each player.
		per_player_count card_cash_award_total{};

		// Number of cash award cards drawn, for each player.
		per_player_count card_cash_award_count{};

		// Total cash paid as a result of immediate cash fee cards, for each player.
		per_player_count card_cash_fee_total{};

		// Number of cash fee cards drawn, for each player.
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

		// Mean rent paid.
		auto rent_paid_mean() const {
			return mean_stat(*this,
				[](statistics_t const& s) { return s.rent_paid_total; },
				[](statistics_t const& s) { return s.rent_paid_count; });
		}

		// Mean rent received.
		auto rent_received_mean() const {
			return mean_stat(*this,
				[](statistics_t const& s) { return s.rent_received_total; },
				[](statistics_t const& s) { return s.rent_received_count; });
		}

		// Relative frequency of board space over all games.
		double board_space_relative_freq(unsigned const board_space) const {
			return static_cast<double>(board_space_counts[board_space]) / position_count;
		}

		// Means times player is sent to jail.
		auto sent_to_jail_count_mean() const {
			return mean_stat(*this, [](statistics_t const& s) { return s.sent_to_jail_count; });
		}

		// Total number of times any player was sent to jail.
		unsigned long long sent_to_jail_count_total() const {
			return std::accumulate(sent_to_jail_count.cbegin(), sent_to_jail_count.cend(), 0ull);
		}

		// Mean duration in turns of being in jail, across all times a player is sent to jail.
		double jail_duration_mean() const {
			return static_cast<double>(jail_duration_total) / sent_to_jail_count_total();
		}

		// Mean number of cards drawn.
		auto cards_drawn_mean() const {
			return mean_stat(*this, [](statistics_t const& s) { return s.cards_drawn; });
		}

		// Mean cash received from immediate cash award cards.
		auto card_cash_award_mean() const {
			return mean_stat<per_draw_mean_stat_mixin>(*this,
				[](statistics_t const& s) { return s.card_cash_award_total; },
				[](statistics_t const& s) { return s.card_cash_award_count; });
		}

		// Mean cash payed to immediate cash fee cards.
		auto card_cash_fee_mean() const {
			return mean_stat<per_draw_mean_stat_mixin>(*this,
				[](statistics_t const& s) { return s.card_cash_fee_total; },
				[](statistics_t const& s) { return s.card_cash_fee_count; });
		}
	};

	inline statistics_t statistics;

}
