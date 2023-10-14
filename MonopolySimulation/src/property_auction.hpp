#pragma once

#include <algorithm>

#include "common_constants.hpp"
#include "common_types.hpp"
#include "game_state.hpp"
#include "player_strategy.hpp"
#include "property_buy.hpp"
#include "random.hpp"
#include "statistics_counters.hpp"


namespace monopoly {

	template<PropertyType P>
	void auction_property(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			P const property) {
		// There doesn't seem to be any info on how exactly auctions are carried out.
		// What is implemented here is:
		//   - Players are queried for their bids in a round robin fashion.
		//   - Players may not lower their bids (as this would allow for price manipulation).
		//   - Players may not bid higher than they have cash on hand.
		//   - Players may not remove their bids once placed.
		//   - The auction ends when there is a round where no player changes their bid.
		//   - If multiple players have bid the same price, the property is not sold.

		auction_state_t auction_state;

		while (true) {
			bool bids_changed = false;
			for (auto const player : players) {
				auto const old_bid = auction_state.bids[player];
				auto const new_bid = strategies.visit(player,
					[&game_state, &random, &auction_state, property](PlayerStrategy auto& strategy) {
						return strategy.bid_on_unowned_property(game_state, random, property, auction_state);
					});
				auto const bid_improved = new_bid > old_bid;
				auto const can_afford_bid = new_bid <= game_state.players[player].cash;
				if (bid_improved && can_afford_bid) {
					auction_state.bids[player] = new_bid;
					bids_changed = true;
				}
			}
			if (!bids_changed) {
				break;
			}
		}

		auto const best_bid_it = std::ranges::max_element(auction_state.bids);
		auto const best_bid_price = *best_bid_it;
		if (best_bid_price != 0) {
			auto const best_bid_count = std::ranges::count(auction_state.bids, best_bid_price);
			if (best_bid_count == 1) {
				auto const best_bid_player = static_cast<unsigned>(best_bid_it - auction_state.bids.cbegin());
				buy_unowned_property(game_state, best_bid_player, property, best_bid_price);

				if (record_stats) {
					auto const property_idx = static_cast<unsigned>(property);
					stat_counters.property_unowned_auction_price.get<P>()[property_idx] += best_bid_price;
					stat_counters.property_unowned_auction_count.get<P>()[property_idx]++;
					stat_counters.unowned_property_auctions_won[best_bid_player]++;
				}
			}
		}
	}

}
