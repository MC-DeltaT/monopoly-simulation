#pragma once

#include <array>

#include "common_types.hpp"


namespace monopoly {

	// These must be sequential starting from 0. Order is not significant.
	enum class chance_card_t : unsigned {
		// Advance to Go.
		advance_to_go,
		// Take a trip to Kings Cross Station.
		advance_to_kings_cross,
		// Advance to Pall Mall.
		advance_to_pall_mall,
		// Advance to Trafalgar Square.
		advance_to_trafalgar_square,
		// Advance to Mayfair.
		advance_to_mayfair,
		// Advance to next railway, pay 2x rent.
		advance_to_next_railway_1,
		// Advance to next railway, pay 2x rent.
		advance_to_next_railway_2,
		// Advance to next utility, pay 10x dice.
		advance_to_next_utility,
		// Go back 3 spaces.
		go_back_3_spaces,
		// Go to jail.
		go_to_jail,
		// Get Out Of Jail Free.
		get_out_of_jail_free,
		// Bank dividend, +$50.
		bank_dividend,
		// Building loan matures, +$150.
		building_loan_matures,
		// Speeding fine, -15.
		speeding_fine,
		// Elected Chairman of the board, -$50 to other players.
		elected_chairman,
		// General repairs on properties, -$25/house, -$100/hotel.
		repairs_on_properties
	};


	// These must be sequential starting from 0. Order is not significant.
	enum class community_chest_card_t : unsigned {
		// Advance to Go.
		advance_to_go,
		// Go to jail.
		go_to_jail,
		// Get Out Of Jail Free.
		get_out_of_jail_free,
		// Won 2nd prize in beauty contest, +$10.
		won_beauty_contest,
		// Income tax refund, +$20.
		income_tax_refund,
		// Consultancy fee, +$25.
		collect_consultancy_fee,
		// Sale of stock, +$50
		sale_of_stock,
		// Inheritance, +$100.
		inheritance,
		// Holiday fund matures, +$100.
		holiday_fund_matures,
		// Life insurance matures, +$100.
		life_insurance_matures,
		// Bank error in your favour, +$200.
		bank_error,
		// Your birthday, +$10 from other players.
		your_birthday,
		// School fees, -$50.
		school_fees,
		// Doctor's fee, -$50.
		doctors_fee,
		// Hospital fees, -$100.
		hospital_fee,
		// Street repairs, -$40/house, -$115/hotel.
		street_repairs
	};


	// Maps a card deck type to its Get Out Of Jail Free card.
	template<card_type_t C>
	inline constexpr auto get_out_of_jail_free_card = nullptr;

	template<> inline constexpr auto get_out_of_jail_free_card<card_type_t::chance> =
		chance_card_t::get_out_of_jail_free;

	template<> inline constexpr auto get_out_of_jail_free_card<card_type_t::community_chest> =
		community_chest_card_t::get_out_of_jail_free;


	inline constexpr std::array<card_type_t, 2> card_types{
		card_type_t::chance,
		card_type_t::community_chest
	};

}