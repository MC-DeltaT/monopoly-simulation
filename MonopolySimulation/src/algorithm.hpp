#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <numeric>


namespace monopoly {

	template<typename T, std::size_t N, typename F = std::identity>
	std::array<std::size_t, N> sorted_indices(std::array<T, N> const& data, F transform = std::identity{}) {
		std::array<std::size_t, N> indices;
		std::iota(indices.begin(), indices.end(), std::size_t{0});
		std::ranges::sort(indices, {}, [&data, &transform](std::size_t const i) {
			return transform(data[i]);
		});
		return indices;
	}


	template<typename T, std::size_t N>
	constexpr bool all_unique(std::array<T, N> const& arr) {
		for (std::size_t i = 0; i < arr.size(); ++i) {
			for (std::size_t j = 0; j < arr.size(); ++j) {
				if (i != j && arr[i] == arr[j]) {
					return false;
				}
			}
		}
		return true;
	}

}
