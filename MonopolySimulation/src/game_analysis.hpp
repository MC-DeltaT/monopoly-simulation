#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <numeric>

#include "common_constants.hpp"
#include "game_state.hpp"
#include "property_constants.hpp"
#include "property_query.hpp"
#include "property_values.hpp"
#include "safe_numeric.hpp"
#include "statistics_counters.hpp"


namespace monopoly {

	[[nodiscard]]
	std::array<unsigned long long, player_count> player_net_worths(game_state_t const& game_state) {
		std::array<unsigned long long, player_count> net_worths{};

		// Assume net worth consists of:
		//   - Cash
		//   - Unmortgaged streets, listed purchase price
		//   - Mortgaged streets, half listed purchase price
		//   - Unmortgaged railways, listed purchase price
		//   - Mortgaged railways, half listed purchase price
		//   - Unmortgaged utilities, listed purchase price
		//   - Mortgaged utilities, half listed purchase price
		//   - Houses, purchase price
		//   - Hotels, purchase price + 4x house purchase price

		// Cash on hand.
		for (auto const player : players) {
			safe_uint_add(net_worths[player], game_state.players[player].cash);
		}

		// Streets and buildings.
		for (auto const& street : streets) {
			if (auto const owner = game_state.street_ownership.get_owner(street); owner.has_value()) {
				if (game_state.street_development.is_mortgaged(street)) {
					safe_uint_add(net_worths[*owner], property_mortgage_value(street));
				}
				else {
					auto const listed_value = street_values[street.global_index];
					auto const building_level = game_state.street_development.building_level(street);
					// Note that a hotel is equivalent to 5 houses.
					auto const value = listed_value + building_values[street.colour_set] * building_level;
					safe_uint_add(net_worths[*owner], value);
				}
			}
		}

		// Railways.
		for (auto const railway : railways) {
			if (auto const owner = game_state.railway_ownership.get_owner(railway); owner.has_value()) {
				auto const value =
					game_state.railway_development.is_mortgaged(railway) ? railway_mortgage_value : railway_value;
				safe_uint_add(net_worths[*owner], value);
			}
		}

		// Utilities.
		for (auto const utility : utilities) {
			if (auto const owner = game_state.utility_ownership.get_owner(utility); owner.has_value()) {
				auto const value =
					game_state.utility_development.is_mortgaged(utility) ? utility_mortgage_value : utility_value;
				safe_uint_add(net_worths[*owner], value);
			}
		}

		// A player should have 0 net worth if they're bankrupt, otherwise something has gone wrong.
		for (auto const player : players) {
			if (game_state.players[player].is_bankrupt()) {
				assert(net_worths[player] == 0);
			}
		}

		return net_worths;
	}


	// Computes the leaderboard rank for each player based on net worth.
	[[nodiscard]]
	std::array<unsigned, player_count> rank_players(game_state_t const& game_state) {
		auto const net_worths = player_net_worths(game_state);

		// Returns true if player lhs ranks better than player rhs.
		auto const rank_predicate = [&game_state, &net_worths](unsigned const lhs, unsigned const rhs) {
			auto const lhs_bankrupt_round = game_state.players[lhs].bankrupt_round;
			auto const rhs_bankrupt_round = game_state.players[rhs].bankrupt_round;
			if (lhs_bankrupt_round.has_value() && rhs_bankrupt_round.has_value()) {
				// Both are bankrupt - higher bankrupt round ranks better.
				return *lhs_bankrupt_round > *rhs_bankrupt_round;
			}
			else if (lhs_bankrupt_round.has_value()) {
				// LHS is bankrupt and RHS is not - RHS ranks better.
				return false;
			}
			else if (rhs_bankrupt_round.has_value()) {
				// RHS is bankrupt and LHS is not - LHS ranks better.
				return true;
			}
			else {
				// Neither is bankrupt - higher net worth ranks better.
				return net_worths[lhs] > net_worths[rhs];
			}
		};

		// Players sorted by rank (better ranks first).
		std::array<unsigned, player_count> players_by_rank;
		std::iota(players_by_rank.begin(), players_by_rank.end(), 0u);
		std::ranges::sort(players_by_rank, rank_predicate);
		
		std::array<unsigned, player_count> player_ranks;
		unsigned rank = 0;
		int prev_player = -1;
		for (auto const player : players_by_rank) {
			// If players have drawn, give the same rank.
			if (prev_player >= 0 && rank_predicate(prev_player, player)) {
				// This player is actually ranked worse.
				++rank;
			}
			player_ranks[player] = rank;
			prev_player = player;
		}

		return player_ranks;
	}

	void game_end_analysis(game_state_t const& game_state) {
		auto const net_worths = player_net_worths(game_state);

		auto const player_rankings = rank_players(game_state);
		for (auto const player : players) {
			stat_counters.player_rank[player] += player_rankings[player];
		}

		for (auto const player : players) {
			stat_counters.final_net_worth[player] += net_worths[player];
		}
	}

}
