#pragma once

#include <cassert>

#include "board_space_constants.hpp"
#include "card_constants.hpp"
#include "card_effects.hpp"
#include "common_types.hpp"
#include "game_state.hpp"
#include "player_strategy.hpp"
#include "random.hpp"


namespace monopoly {

	inline void on_card(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, chance_card_t const card) {
		auto const& player_state = game_state.players[player];
		assert(!player_state.is_bankrupt());
		assert(!player_state.in_jail());

		switch (card) {
		case chance_card_t::advance_to_go:
			card_effects::advance_to_go(game_state, strategies, random, player);
			break;
		case chance_card_t::advance_to_kings_cross:
			card_effects::advance_to_space(game_state, strategies, random, player, board_space_t::kings_cross_station);
			break;
		case chance_card_t::advance_to_pall_mall:
			card_effects::advance_to_space(game_state, strategies, random, player, board_space_t::pall_mall);
			break;
		case chance_card_t::advance_to_trafalgar_square:
			card_effects::advance_to_space(game_state, strategies, random, player, board_space_t::trafalgar_square);
			break;
		case chance_card_t::advance_to_mayfair:
			card_effects::advance_to_space(game_state, strategies, random, player, board_space_t::mayfair);
			break;
		case chance_card_t::advance_to_next_railway_1:
		case chance_card_t::advance_to_next_railway_2:
			card_effects::advance_to_next_railway(game_state, strategies, random, player);
			break;
		case chance_card_t::advance_to_next_utility:
			card_effects::advance_to_next_utility(game_state, strategies, random, player);
			break;
		case chance_card_t::go_back_3_spaces:
			card_effects::go_back_3_spaces(game_state, strategies, random, player);
			break;
		case chance_card_t::go_to_jail:
			card_effects::go_to_jail(game_state, player);
			break;
		case chance_card_t::get_out_of_jail_free:
			card_effects::receive_get_out_of_jail_free(game_state, player, card_type_t::chance);
			break;
		case chance_card_t::bank_dividend:
			card_effects::cash_award(game_state, player, 50u);
			break;
		case chance_card_t::building_loan_matures:
			card_effects::cash_award(game_state, player, 150u);
			break;
		case chance_card_t::speeding_fine:
			card_effects::cash_fee(game_state, strategies, random, player, 15u);
			break;
		case chance_card_t::elected_chairman:
			card_effects::cash_fee_to_players(game_state, strategies, random, player, 50u);
			break;
		case chance_card_t::repairs_on_properties:
			card_effects::per_building_cash_fee(game_state, strategies, random, player, 25u, 100u);
			break;
		default:
			assert(false);
			break;
		}
	}

	inline void on_card(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player, community_chest_card_t const card) {
		auto const& player_state = game_state.players[player];
		assert(!player_state.is_bankrupt());
		assert(!player_state.in_jail());

		switch (card) {
		case community_chest_card_t::advance_to_go:
			card_effects::advance_to_go(game_state, strategies, random, player);
			break;
		case community_chest_card_t::go_to_jail:
			card_effects::go_to_jail(game_state, player);
			break;
		case community_chest_card_t::get_out_of_jail_free:
			card_effects::receive_get_out_of_jail_free(game_state, player, card_type_t::community_chest);
			break;
		case community_chest_card_t::won_beauty_contest:
			card_effects::cash_award(game_state, player, 10u);
			break;
		case community_chest_card_t::income_tax_refund:
			card_effects::cash_award(game_state, player, 20u);
			break;
		case community_chest_card_t::collect_consultancy_fee:
			card_effects::cash_award(game_state, player, 25u);
			break;
		case community_chest_card_t::sale_of_stock:
			card_effects::cash_award(game_state, player, 50u);
			break;
		case community_chest_card_t::inheritance:
			card_effects::cash_award(game_state, player, 100u);
			break;
		case community_chest_card_t::holiday_fund_matures:
			card_effects::cash_award(game_state, player, 100u);
			break;
		case community_chest_card_t::life_insurance_matures:
			card_effects::cash_award(game_state, player, 100u);
			break;
		case community_chest_card_t::bank_error:
			card_effects::cash_award(game_state, player, 200u);
			break;
		case community_chest_card_t::your_birthday:
			card_effects::cash_award_from_players(game_state, strategies, random, player, 10u);
			break;
		case community_chest_card_t::school_fees:
			card_effects::cash_fee(game_state, strategies, random, player, 50u);
			break;
		case community_chest_card_t::doctors_fee:
			card_effects::cash_fee(game_state, strategies, random, player, 50u);
			break;
		case community_chest_card_t::hospital_fee:
			card_effects::cash_fee(game_state, strategies, random, player, 100u);
			break;
		case community_chest_card_t::street_repairs:
			card_effects::per_building_cash_fee(game_state, strategies, random, player, 40u, 115u);
			break;
		default:
			assert(false);
			break;
		}
	}

}
