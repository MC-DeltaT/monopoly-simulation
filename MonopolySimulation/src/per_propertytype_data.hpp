#pragma once

#include <concepts>

#include "common_types.hpp"


namespace monopoly {

	// Provides a uniform interface for accessing per-property-type data by PropertyType.
	template<typename StreetType, typename RailwayType = StreetType, typename UtilityType = StreetType>
	struct per_propertytype_data {
		StreetType street;
		RailwayType railway;
		UtilityType utility;

		template<PropertyType P>
		[[nodiscard]]
		constexpr auto& get() noexcept {
			if constexpr (std::same_as<P, street_t>) {
				return street;
			}
			else if constexpr (std::same_as<P, railway_t>) {
				return railway;
			}
			else if constexpr (std::same_as<P, utility_t>) {
				return utility;
			}
		}

		template<PropertyType P>
		[[nodiscard]]
		constexpr auto const& get() const noexcept {
			if constexpr (std::same_as<P, street_t>) {
				return street;
			}
			else if constexpr (std::same_as<P, railway_t>) {
				return railway;
			}
			else if constexpr (std::same_as<P, utility_t>) {
				return utility;
			}
		}
	};

}
