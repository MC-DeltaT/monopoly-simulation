#pragma once

#include <cstdint>
#include <limits>
#include <utility>


namespace monopoly {

	class random_t {
	public:
		// Based on the extremely fast xorshift64 random number generator.
		// Code sourced from https://en.wikipedia.org/wiki/Xorshift

		using result_type = std::uint64_t;

		explicit constexpr random_t(std::uint64_t const seed) noexcept :
			// Seed of 0 produces infinite sequence of 0s.
			_state{seed | 1u}
		{}

		constexpr random_t& operator=(random_t&&) noexcept = default;

		[[nodiscard]]
		constexpr std::uint64_t operator()() noexcept {
			auto s = _state;
			s ^= s << 13u;
			s ^= s >> 7u;
			s ^= s << 17u;
			_state = s;
			return s;
		}

		[[nodiscard]]
		constexpr std::pair<unsigned, bool> double_dice_roll() noexcept {
			auto const r = (*this)();
			auto const dice1 = static_cast<unsigned>(r % 6u);
			auto const dice2 = static_cast<unsigned>((r >> 8u) % 6u);
			return {dice1 + dice2 + 2u, dice1 == dice2};
		}

		[[nodiscard]]
		constexpr unsigned single_dice_roll() noexcept {
			auto const r = (*this)();
			return (r % 6u) + 1u;
		}

		// Generates a float in the range [0, 1).
		[[nodiscard]]
		constexpr float unit_float() noexcept {
			return ((*this)() % 65536u) / 65536.0f;
		}

		[[nodiscard]]
		constexpr bool uniform_bool() noexcept {
			return (*this)() & 1u;
		}

		[[nodiscard]]
		constexpr bool biased_bool(float const true_probability) noexcept {
			return unit_float() < true_probability;
		}

		[[nodiscard]]
		static constexpr std::uint64_t min() noexcept { return 1; }

		[[nodiscard]]
		static constexpr std::uint64_t max() noexcept { return std::numeric_limits<std::uint64_t>::max(); }

	private:
		std::uint64_t _state;

		// Copying in other contexts is most likely a mistake.
		random_t(random_t const&) noexcept = default;
		random_t& operator=(random_t const&) noexcept = default;
	};

}
