#pragma once

/**
 * @file align.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Defines alignment enums and calculation logic for UI layout.
 * @version 1.1
 * @date 2025-11-29
 * * @details Provides `Align` class to calculate coordinates of an element 
 * within a container based on Main and Cross axes (Flexbox-style logic).
 */

#include "zwidget/unit/point.hpp"
#include "zwidget/unit/size.hpp"

namespace zuu::widget {

    /**
     * @brief Defines the layout direction.
     */
    enum class orientations : uint8_t {
        none,       ///< No specific orientation (fallback to default).
        vertical,   ///< Top-to-bottom layout (Main Axis = Y).
        horizontal  ///< Left-to-right layout (Main Axis = X).
    };

    /**
     * @brief Defines how content is distributed along an axis.
     */
    enum class aligns : uint8_t {
        start,      ///< Align to the start (Left or Top).
        center,     ///< Align to the center.
        end         ///< Align to the end (Right or Bottom).
    };

    /**
     * @brief A utility class to handle alignment logic.
     * Contains orientation and alignment rules for both axes.
     */
    class Align {
    public :
        orientations orientation = orientations::none; ///< The primary layout direction.
        aligns main_axis = aligns::start;              ///< Alignment along the primary direction.
        aligns cross_axis = aligns::start;             ///< Alignment perpendicular to the primary direction.

        /// @name Constructors & Defaults
        /// @{
        constexpr Align() noexcept = default;
        constexpr Align(const Align&) noexcept = default;
        constexpr Align(Align&&) noexcept = default;
        constexpr Align& operator=(const Align&) noexcept = default;
        constexpr Align& operator=(Align&&) noexcept = default;
        constexpr bool operator==(const Align&) const noexcept = default;
        constexpr bool operator!=(const Align&) const noexcept = default;
        constexpr ~Align() noexcept = default;

        /**
         * @brief Constructs an Align object with specific rules.
         * @param _orientation The layout direction.
         * @param _main The alignment for the main axis.
         * @param _cross The alignment for the cross axis.
         */
        constexpr Align(
            orientations _orientation,
            aligns _main,
            aligns _cross
        ) noexcept
         : orientation(_orientation)
         , main_axis(_main), cross_axis(_cross) {}
        /// @}

        /**
         * @brief Calculates the top-left position of content within a container.
         * * @tparam T Numeric type for coordinates.
         * @param container_size The size of the parent/container.
         * @param content_size The size of the item to be aligned.
         * @return basic_point<T> The calculated (x, y) coordinates.
         */
        template <_meta::Numeric T>
        constexpr basic_point<T> compute_position(
            const basic_size<T>& container_size,
            const basic_size<T>& content_size
        ) const noexcept {
            
            // Helper lambda untuk hitung offset 1 dimensi
            auto calculate_offset
			 = [](aligns type, T cont_dim, T item_dim) constexpr -> T {
                switch (type) {
                    case aligns::center: 
                        return (cont_dim - item_dim) / 2;
                    case aligns::end:    
                        return cont_dim - item_dim;
                    case aligns::start:
                    default:             
                        return 0;
                }
            };

            T x = 0;
            T y = 0;

            auto cw = static_cast<T>(container_size.w);
            auto ch = static_cast<T>(container_size.h);
            auto iw = static_cast<T>(content_size.w);
            auto ih = static_cast<T>(content_size.h);

            if (orientation == orientations::horizontal) {
                x = calculate_offset(main_axis, cw, iw);
                y = calculate_offset(cross_axis, ch, ih);
            } else {
                x = calculate_offset(cross_axis, cw, iw);
                y = calculate_offset(main_axis, ch, ih);
            }

            return basic_point<T>{x, y};
        }
    };

} // namespace zuu::widget
