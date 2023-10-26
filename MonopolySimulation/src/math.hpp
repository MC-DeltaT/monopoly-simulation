#pragma once

#include <array>
#include <cstddef>
#include <numeric>
#include <ranges>
#include <type_traits>


namespace monopoly {

	template<typename T1, typename T2>
	[[nodiscard]]
	constexpr double div(T1 const lhs, T2 const rhs) {
		return static_cast<double>(lhs) / static_cast<double>(rhs);
	}

	[[nodiscard]]
	constexpr double cpow(double base, unsigned exponent) {
		double result = 1;
		for (unsigned i = 0; i < exponent; ++i) {
			result *= base;
		}
		return result;
	}

	template<typename T = void, std::ranges::range R>
	[[nodiscard]]
	constexpr auto sum(R const& r) {
		using I = std::conditional_t<std::is_void_v<T>, std::ranges::range_value_t<R>, T>;
		return std::accumulate(std::ranges::begin(r), std::ranges::end(r), I{});
	}

	template<std::size_t N> requires (N > 0)
	[[nodiscard]]
	constexpr double average(std::array<double, N> const& values) {
		return sum<double>(values) / values.size();
	}

	template<typename... Ts> requires (sizeof...(Ts) > 0)
	[[nodiscard]]
	constexpr double average(Ts... values) {
		return average(std::array{static_cast<double>(values)...});
	}
}
