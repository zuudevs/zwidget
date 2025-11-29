#pragma once

/**
 * @file rect.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Defines the basic_rect class for 2D geometry.
 * @version 1.1
 * @date 2025-11-29
 * 
 * @details This header provides a generic rectangle structure defined by an
 * origin point (top-left) and a size (width/height). 
 * Includes saturated arithmetic safety via basic_size integration.
 */

#include "zwidget/unit/point.hpp"
#include "zwidget/unit/size.hpp"

namespace zuu::widget {

    /**
     * @brief A template class representing a 2D rectangle.
     * 
	 * @tparam Tpos The numeric type for position coordinates (e.g., int, float).
     * @tparam Tsize The numeric type for dimensions (e.g., int, float).
     */
    template <_meta::Numeric Tpos, _meta::Numeric Tsize>
    class basic_rect {
    public :
        using pos_type = typename basic_point<Tpos>::base_type;
        using size_type = typename basic_size<Tsize>::base_type;

        basic_point<Tpos> pos {};  ///< The top-left origin of the rectangle.
        basic_size<Tsize> size {}; ///< The dimensions (width and height).

        /// @name Constructors & Defaults
        /// @{

        constexpr basic_rect() noexcept = default;
        constexpr basic_rect(const basic_rect&) noexcept = default;
        constexpr basic_rect(basic_rect&&) noexcept = default;
        constexpr basic_rect& operator=(const basic_rect&) noexcept = default;
        constexpr basic_rect& operator=(basic_rect&&) noexcept = default;
        constexpr auto operator<=>(const basic_rect&) const noexcept = default;
        constexpr ~basic_rect() noexcept = default;

        /**
         * @brief Constructs a rectangle from raw coordinates and dimensions.
         * @param x The X coordinate of the top-left corner.
         * @param y The Y coordinate of the top-left corner.
         * @param w The width.
         * @param h The height.
         */
        template <_meta::Numeric TP, _meta::Numeric TS>
        constexpr basic_rect(TP x, TP y, TS w, TS h) noexcept
         : pos(x, y), size(w, h) {}

        /**
         * @brief Constructs a rectangle from a Point and a Size object.
         */
        template <_meta::Numeric TP, _meta::Numeric TS>
        constexpr basic_rect(const basic_point<TP>& _pos, const basic_size<TS>& _size) noexcept
         : pos(_pos), size(_size) {}
        
        /**
         * @brief Constructs a rectangle from a Size object only.
         * Sets position to (0, 0) by default.
         * @param _size The size of the rectangle.
         */
        template <_meta::Numeric TS>
        explicit constexpr basic_rect(const basic_size<TS>& _size) noexcept
         : pos(0, 0), size(_size) {}

        /// @}

        /// @name Geometry Accessors (Read-Only)
        /// Helper methods to access boundary coordinates easily.
        /// @{

        /// @brief Returns the X coordinate of the left edge.
        constexpr Tpos left() const noexcept { return pos.x; }

        /// @brief Returns the Y coordinate of the top edge.
        constexpr Tpos top() const noexcept { return pos.y; }

        /// @brief Returns the X coordinate of the right edge (x + width).
        constexpr Tpos right() const noexcept { return pos.x + static_cast<Tpos>(size.w); }

        /// @brief Returns the Y coordinate of the bottom edge (y + height).
        constexpr Tpos bottom() const noexcept { return pos.y + static_cast<Tpos>(size.h); }

        /// @brief Returns the width of the rectangle.
        constexpr Tsize width() const noexcept { return size.w; }

        /// @brief Returns the height of the rectangle.
        constexpr Tsize height() const noexcept { return size.h; }

        /**
         * @brief Calculates the center point of the rectangle.
         * @return A basic_point representing the center.
         */
        constexpr basic_point<Tpos> center() const noexcept {
            return basic_point<Tpos>(
                pos.x + static_cast<Tpos>(size.w / 2),
                pos.y + static_cast<Tpos>(size.h / 2)
            );
        }

        /// @}

        /// @name Hit Testing & Intersection
        /// Essential logic for UI interaction.
        /// @{

        /**
         * @brief Checks if a specific point is inside the rectangle.
         * Uses half-open interval: [left, right) and [top, bottom).
         * @param p The point to check.
         * @return true if the point is contained, false otherwise.
         */
        template <_meta::Numeric T>
        constexpr bool contains(const basic_point<T>& p) const noexcept {
            return (p.x >= left() && p.x < right() &&
                    p.y >= top()  && p.y < bottom());
        }

        /**
         * @brief Checks if another rectangle intersects with this one.
         * @param other The other rectangle to check against.
         * @return true if they overlap, false otherwise.
         */
        template <_meta::Numeric TP, _meta::Numeric TS>
        constexpr bool intersects(const basic_rect<TP, TS>& other) const noexcept {
            return (left() < static_cast<Tpos>(other.right()) && 
                    right() > static_cast<Tpos>(other.left()) &&
                    top() < static_cast<Tpos>(other.bottom()) && 
                    bottom() > static_cast<Tpos>(other.top()));
        }

        /**
         * @brief Checks if this rectangle is completely empty (width or height <= 0).
         */
        constexpr bool is_empty() const noexcept {
            return size.has_zero() ;
        }

        /// @}

        /// @name Conversion
        /// @{

        /**
         * @brief Converts this rectangle to a rectangle of different numeric types.
         */
        template <_meta::Numeric TP, _meta::Numeric TS>
        constexpr operator basic_rect<TP, TS>() const noexcept {
            return basic_rect<TP, TS>{
                static_cast<basic_point<TP>>(pos), 
                static_cast<basic_size<TS>>(size)
            };
        }
        /// @}

        // --- Compound Assignments ---
        // Note: These rely on basic_point and basic_size implementations.
        // basic_size handles clamping on subtraction automatically.

        template <_meta::Numeric TP, _meta::Numeric TS>
        constexpr basic_rect& operator+=(const basic_rect<TP, TS>& other) noexcept {
            pos += other.pos ;
            size += other.size ;
            return *this ;
        }

        template <_meta::Numeric TP, _meta::Numeric TS>
        constexpr basic_rect& operator-=(const basic_rect<TP, TS>& other) noexcept {
            pos -= other.pos ;
            size -= other.size ;
            return *this ;
        }

        template <_meta::Numeric TP, _meta::Numeric TS>
        constexpr basic_rect& operator*=(const basic_rect<TP, TS>& other) noexcept {
            pos *= other.pos ;
            size *= other.size ;
            return *this ;
        }

        template <_meta::Numeric TP, _meta::Numeric TS>
        constexpr basic_rect& operator/=(const basic_rect<TP, TS>& other) noexcept {
            pos /= other.pos ;
            size /= other.size ;
            return *this ;
        }

        template <_meta::Numeric T>
        constexpr basic_rect& operator+=(T val) noexcept {
            pos += val ;
            size += val ;
            return *this ;
        }

        template <_meta::Numeric T>
        constexpr basic_rect& operator-=(T val) noexcept {
            pos -= val ;
            size -= val ;
            return *this ;
        }

        template <_meta::Numeric T>
        constexpr basic_rect& operator*=(T val) noexcept {
            pos *= val ;
            size *= val ;
            return *this ;
        }

        template <_meta::Numeric T>
        constexpr basic_rect& operator/=(T val) noexcept {
            pos /= val ;
            size /= val ;
            return *this ;
        }
    };

    /**
     * @brief Deduction Guide: Point + Size
     */
    template <typename Tpos, typename Tsize>
    basic_rect(basic_point<Tpos>, basic_size<Tsize>) -> basic_rect<Tpos, Tsize>;

    /**
     * @brief Deduction Guide: Raw Coordinates (x, y, w, h)
     * Deducts type based on x/y for position and w/h for size.
     */
    template <typename Tx, typename Ty, typename Tw, typename Th>
    basic_rect(Tx, Ty, Tw, Th) -> basic_rect<std::common_type_t<Tx, Ty>, std::common_type_t<Tw, Th>>;

    /**
     * @name Global Binary Operators (Rect vs Rect)
     * @{
     */
    template <_meta::Numeric TPlhs, _meta::Numeric TSlhs
            , _meta::Numeric TPrhs, _meta::Numeric TSrhs>
    constexpr auto operator+(const basic_rect<TPlhs, TSlhs>& lhs, const basic_rect<TPrhs, TSrhs>& rhs) noexcept {
        return basic_rect{ lhs.pos + rhs.pos, lhs.size + rhs.size };
    }

    template <_meta::Numeric TPlhs, _meta::Numeric TSlhs
            , _meta::Numeric TPrhs, _meta::Numeric TSrhs>
    constexpr auto operator-(const basic_rect<TPlhs, TSlhs>& lhs, const basic_rect<TPrhs, TSrhs>& rhs) noexcept {
        return basic_rect{ lhs.pos - rhs.pos, lhs.size - rhs.size };
    }

    template <_meta::Numeric TPlhs, _meta::Numeric TSlhs
            , _meta::Numeric TPrhs, _meta::Numeric TSrhs>
    constexpr auto operator*(const basic_rect<TPlhs, TSlhs>& lhs, const basic_rect<TPrhs, TSrhs>& rhs) noexcept {
        return basic_rect{ lhs.pos * rhs.pos, lhs.size * rhs.size };
    }

    template <_meta::Numeric TPlhs, _meta::Numeric TSlhs
            , _meta::Numeric TPrhs, _meta::Numeric TSrhs>
    constexpr auto operator/(const basic_rect<TPlhs, TSlhs>& lhs, const basic_rect<TPrhs, TSrhs>& rhs) noexcept {
        return basic_rect{ lhs.pos / rhs.pos, lhs.size / rhs.size };
    }
    /// @}

    /**
     * @name Global Binary Operators (Rect vs Scalar)
     * @{
     */
    template<_meta::Numeric TPlhs, _meta::Numeric TSlhs, _meta::Numeric Trhs>
    constexpr auto operator+(const basic_rect<TPlhs, TSlhs>& lhs, Trhs rhs) noexcept {
        return basic_rect{ lhs.pos + rhs, lhs.size + rhs };
    }

    template<_meta::Numeric TPlhs, _meta::Numeric TSlhs, _meta::Numeric Trhs>
    constexpr auto operator-(const basic_rect<TPlhs, TSlhs>& lhs, Trhs rhs) noexcept {
        return basic_rect{ lhs.pos - rhs, lhs.size - rhs };
    }

    template<_meta::Numeric TPlhs, _meta::Numeric TSlhs, _meta::Numeric Trhs>
    constexpr auto operator*(const basic_rect<TPlhs, TSlhs>& lhs, Trhs rhs) noexcept {
        return basic_rect{ lhs.pos * rhs, lhs.size * rhs };
    }

    template<_meta::Numeric TPlhs, _meta::Numeric TSlhs, _meta::Numeric Trhs>
    constexpr auto operator/(const basic_rect<TPlhs, TSlhs>& lhs, Trhs rhs) noexcept {
        return basic_rect{ lhs.pos / rhs, lhs.size / rhs };
    }
    /// @}

    /**
     * @name Global Binary Operators (Scalar vs Rect)
     * @{
     */
    template<_meta::Numeric Tlhs, _meta::Numeric TPrhs, _meta::Numeric TSrhs>
    constexpr auto operator+(Tlhs lhs, const basic_rect<TPrhs, TSrhs>& rhs) noexcept {
        return rhs + lhs ;
    }

    template<_meta::Numeric Tlhs, _meta::Numeric TPrhs, _meta::Numeric TSrhs>
    constexpr auto operator*(Tlhs lhs, const basic_rect<TPrhs, TSrhs>& rhs) noexcept {
        return rhs * lhs ;
    }

    // Note: Scalar - Rect doesn't make geometric sense usually, but mathematically checks out
    // resulting in (Scalar - Pos, Scalar - Size).
    template<_meta::Numeric Tlhs, _meta::Numeric TPrhs, _meta::Numeric TSrhs>
    constexpr auto operator-(Tlhs lhs, const basic_rect<TPrhs, TSrhs>& rhs) noexcept {
        // Warning: This constructs a temporary rect where pos=(lhs,lhs) and size=(lhs,lhs)
        // Then subtracts rhs.
        return basic_rect(lhs, lhs, lhs, lhs) - rhs;
    }

    template<_meta::Numeric Tlhs, _meta::Numeric TPrhs, _meta::Numeric TSrhs>
    constexpr auto operator/(Tlhs lhs, const basic_rect<TPrhs, TSrhs>& rhs) noexcept {
        return basic_rect(lhs, lhs, lhs, lhs) / rhs;
    }
    /// @}

    /// @brief Type alias for a rectangle with integer coordinates and dimensions.
    using Rect = basic_rect<int, int>;

    /// @brief Type alias for a rectangle with float coordinates and dimensions.
    using Rectf = basic_rect<float, float>;

} // namespace zuu::widget
