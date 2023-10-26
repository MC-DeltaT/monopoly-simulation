#pragma once

#include <ranges>

#include "common_constants.hpp"
#include "gameplay_constants.hpp"
#include "math.hpp"


namespace monopoly::basic_ev {

	// Probability on each turn that you have to pay the jail fine to be released, if rolling doubles.
	inline constexpr auto jail_fine_chance =
		std::views::iota(0u, max_turns_in_jail) | std::views::reverse | std::views::transform([](unsigned const i) {
			return cpow(5.0 / 6.0, i + 1);
		});

	// Theoretical value of a Get Out Of Jail Free card on each turn in jail.
	inline constexpr auto get_out_of_jail_free_value = jail_fine_chance | std::views::transform([](double const p) {
		return p * jail_release_cost;
	});


	// Very basic approxiation of the expected value of drawing a Chance card at random.
	inline constexpr double chance_card_ev = average<chance_card_count>({
		go_salary,		// Advance to Go, ignoring any board-position-related-EV.
		0, 0, 0, 0, 0, 0, 0, 0,		// Various movement cards, ignore board-position-related-EV.
		// Go to jail, assuming you don't have GOOJF card, ignoring board-position-related-EV, ignoring turn opp cost
		-jail_fine_chance[0] * jail_release_cost,
		get_out_of_jail_free_value[0],	// Get Out Of Jail Free card, assuming it's unowned.
		50, 150,	// Fixed cash awards.
		-15,		// Fixed cash fees.
		-50.0 * player_count,	// Per-player cash fee, assuming no one is bankrupt.
		0		// Per-building cash fee - buildings not implemented yet.
	});

	// Very basic approxiation of the expected value of drawing a Community Chest card at random.
	inline constexpr double community_chest_card_ev = average<community_chest_card_count>({
		go_salary,		// Advance to Go, ignoring any board-position-related-EV.
		// Go to jail, assuming you don't have GOOJF card, ignoring board-position-related-EV, ignoring turn opp cost
		-jail_fine_chance[0] * jail_release_cost,
		get_out_of_jail_free_value[0],	// Get Out Of Jail Free card, assuming it's unowned.
		10, 20, 25, 50, 100, 100, 100, 200,	// Fixed cash awards.
		10 * player_count,	// Per player cash award, assuming no one is bankrupt.
		-50, -50, -100,		// Fixed cash fees.
		0		// Per-building cash fee - buildings not implemented yet.
	});

}
