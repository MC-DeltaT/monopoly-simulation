#pragma once

#include <cassert>
#include <concepts>
#include <cstdint>
#include <limits>
#include <utility>


namespace monopoly {

	template<std::integral I1, std::integral I2>
	constexpr void safe_int_assign(I1& dst, I2 const value) {
		assert(std::cmp_greater_equal(value, std::numeric_limits<I1>::min()));
		assert(std::cmp_less_equal(value, std::numeric_limits<I1>::max()));
		dst = static_cast<I1>(value);
	}

	template<std::unsigned_integral U1, std::unsigned_integral U2>
	constexpr void safe_uint_add(U1& dst, U2 const add) {
		assert(std::cmp_less_equal(std::uintmax_t{dst} + std::uintmax_t{add}, std::numeric_limits<U1>::max()));
		[[maybe_unused]] auto const prev = dst;
		dst += add;
		assert(std::cmp_greater_equal(dst, prev));
	}

	template<std::unsigned_integral U1, std::unsigned_integral U2>
	constexpr void safe_uint_sub(U1& dst, U2 const sub) {
		assert(std::cmp_greater_equal(dst, sub));
		dst -= sub;
	}

}
