#pragma once


namespace monopoly {

	// Customisable.
	inline constexpr unsigned initial_cash = 1500;


	// Customisable.
	inline constexpr unsigned go_salary = 200;


	// Customisable.
	inline constexpr unsigned total_houses = 32;
	// Customisable.
	inline constexpr unsigned total_hotels = 12;


	// Customisable.
	inline constexpr unsigned consecutive_doubles_jail_threshold = 3;
	static_assert(consecutive_doubles_jail_threshold > 0);

	// Customisable.
	inline constexpr unsigned max_turns_in_jail = 3;
	static_assert(max_turns_in_jail > 0);

	// Customisable.
	inline constexpr unsigned jail_release_cost = 50;

}
