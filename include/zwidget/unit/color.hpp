#pragma once

/**
 * @file color.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Defines the Color class for RGBA representation.
 * @version 1.1
 * @date 2025-11-28
 */

#include <algorithm>
#include <cstdint>
#include <concepts>

namespace zuu::widget {

    /**
     * @brief Represents a color in RGBA8888 format.
     * Each channel (Red, Green, Blue, Alpha) is an 8-bit unsigned integer.
     */
    class Color {
    public:
        uint8_t r {}; ///< Red component (0-255)
        uint8_t g {}; ///< Green component (0-255)
        uint8_t b {}; ///< Blue component (0-255)
        uint8_t a {255}; ///< Alpha component (0-255), defaults to opaque.

        /// @name Constructors & Defaults
        /// @{
        constexpr Color() noexcept = default;
        constexpr Color(const Color&) noexcept = default;
        constexpr Color(Color&&) noexcept = default;
        constexpr Color& operator=(const Color&) noexcept = default;
        constexpr Color& operator=(Color&&) noexcept = default;
        constexpr auto operator<=>(const Color&) const noexcept = default;
        constexpr ~Color() noexcept = default;

        /**
         * @brief Constructs a color from RGBA values.
         * @param _r Red (0-255)
         * @param _g Green (0-255)
         * @param _b Blue (0-255)
         * @param _a Alpha (0-255), default is 255 (opaque).
         */
        template <std::integral T>
        constexpr Color(T _r, T _g, T _b, T _a = 255) noexcept
            : r(static_cast<uint8_t>(_r)), g(static_cast<uint8_t>(_g))
            , b(static_cast<uint8_t>(_b)), a(static_cast<uint8_t>(_a)) {}

        /**
         * @brief Constructs a grayscale color.
         * @param val Value for R, G, and B. Alpha is set to 255.
         */
        template <std::integral T>
        explicit constexpr Color(T val) noexcept
            : r(static_cast<uint8_t>(val)), g(static_cast<uint8_t>(val))
            , b(static_cast<uint8_t>(val)), a(255) {}
        /// @}

        /// @name Helper Methods
        /// @{

        /**
         * @brief Returns a copy of this color with a modified alpha value.
         * Useful for fading effects or transparency without altering the original color.
         * @param new_alpha The new alpha value (0-255).
         * @return New Color instance.
         */
        constexpr Color with_alpha(uint8_t new_alpha) const noexcept {
            return Color(r, g, b, new_alpha);
        }
        /// @}

        /// @name Predefined Colors
        /// Standard HTML/CSS colors usually needed in UI development.
        /// Using static functions/variables allows usage like `Color::red()`.
        /// @{
        [[nodiscard]] static consteval Color transparent() { return {0, 0, 0, 0}; }
        [[nodiscard]] static consteval Color black()       { return {0, 0, 0}; }
        [[nodiscard]] static consteval Color white()       { return {255, 255, 255}; }
        [[nodiscard]] static consteval Color red()         { return {255, 0, 0}; }
        [[nodiscard]] static consteval Color green()       { return {0, 255, 0}; }
        [[nodiscard]] static consteval Color blue()        { return {0, 0, 255}; }
        [[nodiscard]] static consteval Color yellow()      { return {255, 255, 0}; }
        [[nodiscard]] static consteval Color cyan()        { return {0, 255, 255}; }
        [[nodiscard]] static consteval Color magenta()     { return {255, 0, 255}; }
        [[nodiscard]] static consteval Color gray()        { return {128, 128, 128}; }
        /// @}
    };

	[[nodiscard]] inline constexpr Color rgba(
		std::unsigned_integral auto r,
		std::unsigned_integral auto g,
		std::unsigned_integral auto b,
		std::floating_point auto a = decltype(a){1}
	) noexcept {
		return {
			static_cast<uint8_t>(r > 255 ? 255 : r),
			static_cast<uint8_t>(g > 255 ? 255 : g),
			static_cast<uint8_t>(b > 255 ? 255 : b),
			static_cast<uint8_t>(
				decltype(a){255} * std::clamp(a, decltype(a){0}, decltype(a){1})
			)
		} ;
	}
} // namespace zuu::widget
