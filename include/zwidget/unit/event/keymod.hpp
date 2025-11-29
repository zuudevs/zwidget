#pragma once

#include <cstdint>
namespace zuu::widget {

	enum class key_modifier : uint8_t {
		none     = 0,
		shift    = 1 << 0,
		ctrl     = 1 << 1,
		alt      = 1 << 2,
		win      = 1 << 3
	};

	constexpr key_modifier operator|(key_modifier a, key_modifier b) noexcept {
		return static_cast<key_modifier>(
			static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
		) ;
	}

	constexpr key_modifier operator&(key_modifier a, key_modifier b) noexcept {
		return static_cast<key_modifier>(
			static_cast<uint8_t>(a) & static_cast<uint8_t>(b)
		) ;
	}

	constexpr bool has_modifier(key_modifier flags, key_modifier check) noexcept {
			return (flags & check) == check;
		}

} // namespace zuu::widget
