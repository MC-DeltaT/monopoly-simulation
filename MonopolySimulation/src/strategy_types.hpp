#pragma once

#include <concepts>

#include "common_types.hpp"
#include "game_state.hpp"
#include "generic_sell_to_bank_iface.hpp"
#include "random.hpp"
#include "static_vector.hpp"


namespace monopoly {

	using sell_to_bank_choices_t = static_vector<generic_sell_to_bank_t, 8>;


	enum class in_jail_action_t {
		pay_fine,
		get_out_of_jail_free_chance,
		get_out_of_jail_free_community_chest,
		roll_doubles
	};


	template<typename T>
	concept PlayerStrategy = requires (T t, game_state_t const game, random_t random,
			auction_state_t const auction, street_t const street, railway_t const railway, utility_t const utility,
			unsigned const u) {
		// Decides if the player should purchase an unowned property they have landed on.
		// Will only be called if the player has enough cash to buy the property.
		{ t.should_buy_unowned_property(game, random, street) } -> std::same_as<bool>;
		{ t.should_buy_unowned_property(game, random, railway) } -> std::same_as<bool>;
		{ t.should_buy_unowned_property(game, random, utility) } -> std::same_as<bool>;

		// Places a bid on an unowned property up for auction.
		// Bids that the player cannot afford or don't improve their previous bid are ignored.
		{ t.bid_on_unowned_property(game, random, street, auction) } -> std::same_as<unsigned>;
		{ t.bid_on_unowned_property(game, random, railway, auction) } -> std::same_as<unsigned>;
		{ t.bid_on_unowned_property(game, random, utility, auction) } -> std::same_as<unsigned>;

		// Decide what to do on a jail turn.
		// Will only be called in the player has enough cash to pay the fine, or if owning a Get Out Of Jail Free card.
		{ t.decide_jail_action(game, random) } -> std::same_as<in_jail_action_t>;

		// Pick assets to sell to generate the specified amount of cash. Assets will be sold in order.
		// Must not pick assets that the player doesn't own or are unsellable.
		// Must not return nothing if the player has sellable assets.
		{ t.choose_assets_for_forced_sale(game, random, u) } -> std::same_as<sell_to_bank_choices_t>;
	};

}
