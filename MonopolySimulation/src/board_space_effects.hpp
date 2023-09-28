#pragma once

#include "card_dispatch.hpp"
#include "card_draw.hpp"
#include "cash.hpp"
#include "common_types.hpp"
#include "game_state.hpp"
#include "movement.hpp"
#include "player_strategy.hpp"
#include "property_auction.hpp"
#include "property_buy.hpp"
#include "random.hpp"
#include "rent.hpp"
#include "statistics_counters.hpp"


namespace monopoly::board_effects {

	void on_owned_property_space(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, PropertyType auto const property) {
		pay_rent(game_state, strategies, random, player, property);
		// Turn ends.
	}

	void on_unowned_property_space(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, PropertyType auto const property) {
		auto const bought = maybe_buy_unowned_property(game_state, strategies, random, player, property);
		if (!bought) {
			auction_property(game_state, strategies, random, property);
		}
		// Turn ends.
	}

	template<PropertyType P>
	void on_property_space(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, P const property) {
		if (game_state.property_ownership<P>().is_owned(property)) {
			on_owned_property_space(game_state, strategies, random, player, property);
		}
		else {
			on_unowned_property_space(game_state, strategies, random, player, property);
		}
	}


	inline void on_go_space(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		// The Go salary is paid previously when the player lands on Go, so nothing is required here.
		// Turn ends.
	}

	inline void on_tax_space(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, unsigned const tax_amount) {
		player_pay_bank(game_state, strategies, random, player, tax_amount);
		// Turn ends.
	}

	inline void on_free_parking(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		// Turn ends.
	}

	inline void on_just_visiting_jail(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		// Turn ends.
	}

	inline void on_go_to_jail(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		go_to_jail(game_state, player);
		// Turn ends.
	}


	inline void on_chance_space(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		auto const card = draw_card<card_type_t::chance>(game_state);
		stat_counters.cards_drawn[player]++;
		on_card(game_state, strategies, random, player, card);
	}

	inline void on_community_chest_space(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		auto const card = draw_card<card_type_t::community_chest>(game_state);
		stat_counters.cards_drawn[player]++;
		on_card(game_state, strategies, random, player, card);
	}

}
