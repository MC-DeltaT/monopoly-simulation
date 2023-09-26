#pragma once

#include <ranges>


namespace monopoly {

	inline constexpr unsigned player_count = 4;
	static_assert(player_count >= 2 && player_count <= 4);

	inline constexpr auto players = std::views::iota(0u, player_count);


	inline constexpr unsigned board_space_count = 40;

	inline constexpr unsigned railway_count = 4;

	inline constexpr unsigned utility_count = 2;


	inline constexpr unsigned chance_card_count = 16;

	inline constexpr unsigned community_chest_card_count = 16;

}
