#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <numeric>
#include <utility>


namespace monopoly {
	
	// Produces array of indices of elements as if they were sorted.
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


	// Bad but fast shuffle.
	template<typename T, std::size_t N>
	void fast_shuffle(std::array<T, N>& arr, auto& random_engine) {
		for (std::size_t i = 0; i < N; ++i) {
			auto const new_idx = random_engine() % N;
			std::swap(arr[i], arr[new_idx]);
		}
	}

}
