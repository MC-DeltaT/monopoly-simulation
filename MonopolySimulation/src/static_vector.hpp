#pragma once

#include <array>
#include <cassert>
#include <concepts>
#include <utility>


namespace monopoly {

	template<std::semiregular T, unsigned N> requires (N > 0)
	class static_vector {
	public:
		constexpr static_vector() :
			_elements{},
			_size{0}
		{}

		constexpr void push_back(T&& element) {
			emplace_back(std::move(element));
		}

		template<typename... Args>
		constexpr void emplace_back(Args&&... args) {
			assert(_size < N);
			_elements[_size] = T{std::forward<Args>(args)...};
			++_size;
		}

		[[nodiscard]]
		constexpr auto begin() const noexcept {
			return _elements.begin();
		}

		[[nodiscard]]
		constexpr auto end() const noexcept {
			return _elements.begin() + _size;
		}

		[[nodiscard]]
		constexpr bool empty() const noexcept {
			return _size == 0;
		}

	private:
		std::array<T, N> _elements;
		unsigned _size;
	};

}
