#pragma once

#include <cassert>

#include "board_space_constants.hpp"
#include "board_space_effects.hpp"
#include "game_state.hpp"
#include "player_strategy.hpp"
#include "property_constants.hpp"
#include "random.hpp"


namespace monopoly {

	inline void on_board_space(game_state_t& game_state, player_strategies_t& strategies, random_t& random,
			unsigned const player) {
		auto& player_state = game_state.players[player];
		assert(!player_state.in_jail());
		assert(!player_state.is_bankrupt());
		
		switch (player_state.get_board_space()) {
		case board_space_t::go:
			board_effects::on_go_space(game_state);
			break;
		case board_space_t::old_kent_road:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<0>(streets));
			break;
		case board_space_t::community_chest_1:
			board_effects::on_community_chest_space(game_state, strategies, random, player);
			break;
		case board_space_t::whitechapel_road:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<1>(streets));
			break;
		case board_space_t::income_tax:
			board_effects::on_tax_space(game_state, strategies, random, player, income_tax);
			break;
		case board_space_t::kings_cross_station:
			board_effects::on_property_space(game_state, strategies, random, player, railway_t::kings_cross);
			break;
		case board_space_t::the_angel_islington:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<2>(streets));
			break;
		case board_space_t::chance_1:
			board_effects::on_chance_space(game_state, strategies, random, player);
			break;
		case board_space_t::euston_road:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<3>(streets));
			break;
		case board_space_t::pentonville_road:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<4>(streets));
			break;
		case board_space_t::just_visiting_jail:
			board_effects::on_just_visiting_jail();
			break;
		case board_space_t::pall_mall:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<5>(streets));
			break;
		case board_space_t::electric_company:
			board_effects::on_property_space(game_state, strategies, random, player, utility_t::electric_company);
			break;
		case board_space_t::whitehall:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<6>(streets));
			break;
		case board_space_t::northumberland_avenue:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<7>(streets));
			break;
		case board_space_t::marylebone_station:
			board_effects::on_property_space(game_state, strategies, random, player, railway_t::marylebone);
			break;
		case board_space_t::bow_street:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<8>(streets));
			break;
		case board_space_t::community_chest_2:
			board_effects::on_community_chest_space(game_state, strategies, random, player);
			break;
		case board_space_t::marlborough_street:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<9>(streets));
			break;
		case board_space_t::vine_street:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<10>(streets));
			break;
		case board_space_t::free_parking:
			board_effects::on_free_parking();
			break;
		case board_space_t::strand:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<11>(streets));
			break;
		case board_space_t::chance_2:
			board_effects::on_chance_space(game_state, strategies, random, player);
			break;
		case board_space_t::fleet_street:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<12>(streets));
			break;
		case board_space_t::trafalgar_square:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<13>(streets));
			break;
		case board_space_t::fenchurch_street_station:
			board_effects::on_property_space(game_state, strategies, random, player, railway_t::fenchurch_street);
			break;
		case board_space_t::leicester_square:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<14>(streets));
			break;
		case board_space_t::coventry_street:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<15>(streets));
			break;
		case board_space_t::water_works:
			board_effects::on_property_space(game_state, strategies, random, player, utility_t::water_works);
			break;
		case board_space_t::piccadilly:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<16>(streets));
			break;
		case board_space_t::go_to_jail:
			board_effects::on_go_to_jail(game_state, player);
			break;
		case board_space_t::regent_street:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<17>(streets));
			break;
		case board_space_t::oxford_street:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<18>(streets));
			break;
		case board_space_t::community_chest_3:
			board_effects::on_community_chest_space(game_state, strategies, random, player);
			break;
		case board_space_t::bond_street:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<19>(streets));
			break;
		case board_space_t::liverpool_street_station:
			board_effects::on_property_space(game_state, strategies, random, player, railway_t::liverpool_street);
			break;
		case board_space_t::chance_3:
			board_effects::on_chance_space(game_state, strategies, random, player);
			break;
		case board_space_t::park_lane:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<20>(streets));
			break;
		case board_space_t::super_tax:
			board_effects::on_tax_space(game_state, strategies, random, player, super_tax);
			break;
		case board_space_t::mayfair:
			board_effects::on_property_space(game_state, strategies, random, player, std::get<21>(streets));
			break;
		default:
			assert(false);
			break;
		}
	}

}
