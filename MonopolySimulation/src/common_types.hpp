#pragma once

#include <concepts>


namespace monopoly {

	enum class board_space_t : unsigned;


	class street_t {
	public:
		constexpr street_t(unsigned const generic_index, unsigned const colour_set, unsigned const index_in_set) noexcept :
			generic_index{generic_index}, colour_set{colour_set}, index_in_set{index_in_set}
		{}

		constexpr street_t(unsigned const generic_index);

		unsigned generic_index;
		unsigned colour_set;
		unsigned index_in_set;

		[[nodiscard]]
		explicit constexpr operator unsigned() const noexcept {
			return generic_index;
		}
	};

	enum class railway_t : unsigned;

	enum class utility_t : unsigned;

	template<typename T>
	concept PropertyType = std::same_as<T, street_t> || std::same_as<T, railway_t> || std::same_as<T, utility_t>;


	// These must be sequential starting from 0. Order is not significant.
	enum class card_type_t : unsigned {
		chance,
		community_chest
	};

	enum class chance_card_t : unsigned;

	enum class community_chest_card_t : unsigned;
}
