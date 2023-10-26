#pragma once

#include <cassert>
#include <concepts>
#include <tuple>
#include <utility>

#include "common_types.hpp"
#include "game_state.hpp"
#include "generic_sell_to_bank_iface.hpp"
#include "property_query.hpp"
#include "property_values.hpp"
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


	// Always use Get Out Of Jail Free if the player has one, otherwise roll doubles.
	struct always_use_card_jail_strategy_t {
		[[nodiscard]]
		in_jail_action_t decide_jail_action(game_state_t const& game_state, random_t&, unsigned const player) const {
			if (game_state.get_out_of_jail_free_ownership.is_owner(player, card_type_t::chance)) {
				return in_jail_action_t::get_out_of_jail_free_chance;
			}
			else if (game_state.get_out_of_jail_free_ownership.is_owner(player, card_type_t::community_chest)) {
				return in_jail_action_t::get_out_of_jail_free_community_chest;
			}
			else {
				return in_jail_action_t::roll_doubles;
			}
		}
	};


	// Always try to roll doubles to get out of jail.
	struct always_roll_jail_strategy_t {
		[[nodiscard]]
		static constexpr in_jail_action_t decide_jail_action(game_state_t const&, random_t&, unsigned const) noexcept {
			return in_jail_action_t::roll_doubles;
		}
	};


	// Fixed probability of buying a property if the player can afford it.
	struct random_unowned_property_buy_strategy_t {
		float buy_probability;

		[[nodiscard]]
		bool should_buy_unowned_property(game_state_t const& game_state, random_t& random, unsigned const player,
				PropertyType auto const property) const {
			auto const property_value = property_buy_cost(property);
			auto const player_cash = game_state.players[player].cash;
			if (player_cash >= property_value) {
				auto const buy = random.biased_bool(buy_probability);
				return buy;
			}
			return false;
		}
	};


	// Never buy unowned property.
	struct dont_buy_unowned_property_buy_strategy_t {
		[[nodiscard]]
		static constexpr bool should_buy_unowned_property(game_state_t const&, random_t&, unsigned const,
				PropertyType auto const) noexcept {
			return false;
		}
	};


	// Bid amount uniformly distributed around property price.
	struct random_unowned_property_bid_strategy_t {
		// mean(bid) = property_price * (1 + centre_adjust)
		// min(bid) = mean(bid) - width * property_price / 2
		// max(bid) = mean(bid) + width * property_price / 2

		float centre_adjust;	// As a fraction of the property price.
		float width;	// As a fraction of the property price.

		[[nodiscard]]
		unsigned bid_on_unowned_property(game_state_t const&, random_t& random, unsigned const player,
				PropertyType auto const property, auction_state_t const& auction) const {
			if (auction.bids[player] == 0) {
				auto const property_price = property_buy_cost(property);
				auto const width_abs = property_price * width;
				auto const mean = property_price * (1 + centre_adjust);
				auto const min = mean - width_abs / 2;
				auto const r = random.unit_float();
				auto const bid = r * width_abs + min;
				// TODO? maybe clamp result
				return static_cast<unsigned>(bid);
			}
			else {
				return 0;
			}
		}
	};


	// Never participate in unowned property auctions.
	struct dont_bid_unowned_property_bid_strategy_t {
		[[nodiscard]]
		static constexpr unsigned bid_on_unowned_property(game_state_t const&, random_t&, unsigned const,
				PropertyType auto const, auction_state_t const&) noexcept {
			return 0;
		}
	};


	struct basic_forced_sale_strategy_t {
		// Sell in this order:
		//   - Streets with no buildings, cheapest first;
		//   - Utilities;
		//   - Railways;
		//   - Buildings, cheapest first.

		[[nodiscard]]
		sell_to_bank_choices_t choose_assets_for_forced_sale(game_state_t const& game_state, random_t&,
				unsigned const player, unsigned const min_amount) const {
			sell_to_bank_choices_t choices;

			long long amount_remaining = min_amount;

			// Assuming streets are already in order of ascending value (true for regular gameplay config).
			for (auto const& street : streets) {
				auto const can_sell = game_state.property_ownership.street.is_owner(player, street)
					&& is_property_sellable(game_state, street);
				if (can_sell) {
					choices.emplace_back(generic_sell_to_bank_type::street, static_cast<unsigned>(street));
					amount_remaining -= property_sell_value(street);
					if (amount_remaining <= 0 || choices.full()) {
						return choices;
					}
				}
			}

			for (auto const utility : utilities) {
				auto const can_sell = game_state.property_ownership.utility.is_owner(player, utility)
					&& is_property_sellable(game_state, utility);
				if (can_sell) {
					choices.emplace_back(generic_sell_to_bank_type::utility, static_cast<unsigned>(utility));
					amount_remaining -= utility_sell_value;
					if (amount_remaining <= 0 || choices.full()) {
						return choices;
					}
				}
			}

			for (auto const railway : railways) {
				auto const can_sell = game_state.property_ownership.railway.is_owner(player, railway)
					&& is_property_sellable(game_state, railway);
				if (can_sell) {
					choices.emplace_back(generic_sell_to_bank_type::railway, static_cast<unsigned>(railway));
					amount_remaining -= railway_sell_value;
					if (amount_remaining <= 0 || choices.full()) {
						return choices;
					}
				}
			}

			// TODO: buildings

			return choices;
		}
	};


	template<
		auto JailStrategy, auto UnownedPropertyBuyStrategy, auto UnownedPropertyBidStrategy, auto ForcedSaleStrategy> 
	struct flexible_player_strategy_t {
		unsigned player;

		[[nodiscard]]
		bool should_buy_unowned_property(game_state_t const& game_state, random_t& random,
				PropertyType auto const property) {
			return UnownedPropertyBuyStrategy.should_buy_unowned_property(game_state, random, player, property);
		}

		[[nodiscard]]
		unsigned bid_on_unowned_property(game_state_t const& game_state, random_t& random,
				PropertyType auto const property, auction_state_t const& auction) {
			return UnownedPropertyBidStrategy.bid_on_unowned_property(game_state, random, player, property, auction);
		}

		[[nodiscard]]
		in_jail_action_t decide_jail_action(game_state_t const& game_state, random_t& random) {
			return JailStrategy.decide_jail_action(game_state, random, player);
		}

		[[nodiscard]]
		sell_to_bank_choices_t choose_assets_for_forced_sale(game_state_t const& game_state, random_t& random,
				unsigned const min_amount) {
			return ForcedSaleStrategy.choose_assets_for_forced_sale(game_state, random, player, min_amount);
		}
	};


	struct player_strategies_t {
		std::tuple<
			flexible_player_strategy_t<
				always_roll_jail_strategy_t{},
				dont_buy_unowned_property_buy_strategy_t{},
				dont_bid_unowned_property_bid_strategy_t{},
				basic_forced_sale_strategy_t{}>,
			flexible_player_strategy_t<
				always_roll_jail_strategy_t{},
				dont_buy_unowned_property_buy_strategy_t{},
				dont_bid_unowned_property_bid_strategy_t{},
				basic_forced_sale_strategy_t{}>,
			flexible_player_strategy_t<
				always_roll_jail_strategy_t{},
				dont_buy_unowned_property_buy_strategy_t{},
				dont_bid_unowned_property_bid_strategy_t{},
				basic_forced_sale_strategy_t{}>,
			flexible_player_strategy_t<
				always_roll_jail_strategy_t{},
				dont_buy_unowned_property_buy_strategy_t{},
				dont_bid_unowned_property_bid_strategy_t{},
				basic_forced_sale_strategy_t{}>
		> strategies{{0}, {1}, {2}, {3}};

		player_strategies_t() = default;
		player_strategies_t& operator=(player_strategies_t&&) = default;

		template<typename F>
		decltype(auto) visit(unsigned const player, F&& func) {
			switch (player) {
			case 0:
				return std::forward<F>(func)(std::get<0>(strategies));
			case 1:
				return std::forward<F>(func)(std::get<1>(strategies));
			case 2:
				return std::forward<F>(func)(std::get<2>(strategies));
			case 3:
				return std::forward<F>(func)(std::get<3>(strategies));
			default:
				assert(false);
			}
			assert(false);
		}

	private:
		// Copying in other contexts is most likely a mistake.
		player_strategies_t(player_strategies_t const&) = default;
		player_strategies_t& operator=(player_strategies_t const&) = default;

		static_assert(PlayerStrategy<std::tuple_element_t<0, decltype(strategies)>>);
		static_assert(PlayerStrategy<std::tuple_element_t<1, decltype(strategies)>>);
		static_assert(PlayerStrategy<std::tuple_element_t<2, decltype(strategies)>>);
		static_assert(PlayerStrategy<std::tuple_element_t<3, decltype(strategies)>>);
	};

}
