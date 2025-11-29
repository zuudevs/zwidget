#pragma once

/**
 * @file point.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Defines the basic_point class and related arithmetic operations.
 * @version 1.0
 * @date 2025-11-28
 * 
 * @details This header provides a generic 2D point structure that supports
 * fundamental vector arithmetic. It leverages C++20 concepts to ensure
 * type safety for numeric operations.
 */

#include <compare>
#include "zwidget/concept/numeric.hpp"

namespace zuu::widget {

    /**
     * @brief A template class representing a 2D point with (x, y) coordinates.
     * 
	 * @tparam Base The underlying numeric type for the coordinates. 
     * Must satisfy the `_meta::Numeric` concept.
     */
    template <_meta::Numeric Base>
    class basic_point {
    public :
        /// @brief The underlying value type.
        using base_type = Base;

        Base x {}; ///< The X coordinate.
        Base y {}; ///< The Y coordinate.

        /// @name Constructors & Destructor
        /// @{

        constexpr basic_point() noexcept = default;
        constexpr basic_point(const basic_point&) noexcept = default;
        constexpr basic_point(basic_point&&) noexcept = default;
        constexpr basic_point& operator=(const basic_point&) noexcept = default;
        constexpr basic_point& operator=(basic_point&&) noexcept = default;
        
        /// @brief Three-way comparison operator (spaceship operator).
        constexpr auto operator<=>(const basic_point&) const noexcept = default;
        
        constexpr ~basic_point() noexcept = default;

        /**
         * @brief Constructs a point with specific x and y coordinates.
         * 
		 * @tparam Tx Type of the x coordinate.
         * @tparam Ty Type of the y coordinate.
         * @param _x The value for the x coordinate.
         * @param _y The value for the y coordinate.
         */
        template <_meta::Numeric Tx, _meta::Numeric Ty>
        constexpr basic_point(Tx _x, Ty _y) noexcept
         : x(static_cast<Base>(_x)), y(static_cast<Base>(_y)) {}

        /**
         * @brief Explicit constructor from a single scalar value.
         * Initializes both x and y with the same value.
         * 
		 * @tparam T The type of the scalar value.
         * @param val The value to assign to both x and y.
         */
        template <_meta::Numeric T>
        explicit constexpr basic_point(T val) noexcept
         : x(static_cast<Base>(val)), y(static_cast<Base>(val)) {}

        /// @}

        /// @name Conversions & Compound Assignment
        /// @{

        /**
         * @brief Type conversion operator to a basic_point of a different type.
         * 
		 * @tparam T The target numeric type.
         * @return A new basic_point cast to type T.
         */
        template <_meta::Numeric T>
        constexpr operator basic_point<T>() const noexcept {
            return basic_point<T>{static_cast<T>(x), static_cast<T>(y)};
        }

        /**
         * @brief Component-wise addition assignment.
         */
        template <_meta::Numeric T>
        constexpr basic_point& operator+=(const basic_point<T>& rhs) noexcept {
            x += static_cast<Base>(rhs.x);
            y += static_cast<Base>(rhs.y);
            return *this;
        }

        /**
         * @brief Component-wise subtraction assignment.
         */
        template <_meta::Numeric T>
        constexpr basic_point& operator-=(const basic_point<T>& rhs) noexcept {
            x -= static_cast<Base>(rhs.x);
            y -= static_cast<Base>(rhs.y);
            return *this;
        }

        /**
         * @brief Component-wise multiplication assignment.
         */
        template <_meta::Numeric T>
        constexpr basic_point& operator*=(const basic_point<T>& rhs) noexcept {
            x *= static_cast<Base>(rhs.x);
            y *= static_cast<Base>(rhs.y);
            return *this;
        }

        /**
         * @brief Component-wise division assignment.
         */
        template <_meta::Numeric T>
        constexpr basic_point& operator/=(const basic_point<T>& rhs) noexcept {
            x /= static_cast<Base>(rhs.x);
            y /= static_cast<Base>(rhs.y);
            return *this;
        }

        // --- Scalar assignment operators ---

        /**
         * @brief Adds a scalar value to both coordinates.
         */
        template <_meta::Numeric U>
        constexpr basic_point& operator+=(U val) noexcept {
            x += static_cast<Base>(val);
            y += static_cast<Base>(val);
            return *this;
        }

        /**
         * @brief Subtracts a scalar value from both coordinates.
         */
        template <_meta::Numeric U>
        constexpr basic_point& operator-=(U val) noexcept {
            x -= static_cast<Base>(val);
            y -= static_cast<Base>(val);
            return *this;
        }

        /**
         * @brief Multiplies both coordinates by a scalar value.
         */
        template <_meta::Numeric U>
        constexpr basic_point& operator*=(U val) noexcept {
            x *= static_cast<Base>(val);
            y *= static_cast<Base>(val);
            return *this;
        }

        /**
         * @brief Divides both coordinates by a scalar value.
         */
        template <_meta::Numeric U>
        constexpr basic_point& operator/=(U val) noexcept {
            x /= static_cast<Base>(val);
            y /= static_cast<Base>(val);
            return *this;
        }
        /// @}
    };

    /**
     * @name Global Binary Operators (Point vs Point)
     * Operations between two points. The result type is deduced using `std::common_type`.
     * @{
     */

    template <_meta::Numeric Tlhs, _meta::Numeric Trhs>
    constexpr auto operator+(const basic_point<Tlhs>& a, const basic_point<Trhs>& b) noexcept {
        using Tres = std::common_type_t<Tlhs, Trhs>;
        return basic_point<Tres>{
            static_cast<Tres>(a.x) + static_cast<Tres>(b.x),
            static_cast<Tres>(a.y) + static_cast<Tres>(b.y)
        };
    }

    template <_meta::Numeric Tlhs, _meta::Numeric Trhs>
    constexpr auto operator-(const basic_point<Tlhs>& a, const basic_point<Trhs>& b) noexcept {
        using Tres = std::common_type_t<Tlhs, Trhs>;
        return basic_point<Tres>{
            static_cast<Tres>(a.x) - static_cast<Tres>(b.x),
            static_cast<Tres>(a.y) - static_cast<Tres>(b.y)
        };
    }

    template <_meta::Numeric Tlhs, _meta::Numeric Trhs>
    constexpr auto operator*(const basic_point<Tlhs>& a, const basic_point<Trhs>& b) noexcept {
        using Tres = std::common_type_t<Tlhs, Trhs>;
        return basic_point<Tres>{
            static_cast<Tres>(a.x) * static_cast<Tres>(b.x),
            static_cast<Tres>(a.y) * static_cast<Tres>(b.y)
        };
    }

    template <_meta::Numeric Tlhs, _meta::Numeric Trhs>
    constexpr auto operator/(const basic_point<Tlhs>& a, const basic_point<Trhs>& b) noexcept {
        using Tres = std::common_type_t<Tlhs, Trhs>;
        return basic_point<Tres>{
            static_cast<Tres>(a.x) / static_cast<Tres>(b.x),
            static_cast<Tres>(a.y) / static_cast<Tres>(b.y)
        };
    }
    /// @}

    /**
     * @name Global Binary Operators (Point vs Scalar)
     * Operations between a point and a scalar.
     * @{
     */

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator+(const basic_point<T>& lhs, U rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_point<Tres>{
            static_cast<Tres>(lhs.x) + static_cast<Tres>(rhs),
            static_cast<Tres>(lhs.y) + static_cast<Tres>(rhs)
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator-(const basic_point<T>& lhs, U rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_point<Tres>{
            static_cast<Tres>(lhs.x) - static_cast<Tres>(rhs),
            static_cast<Tres>(lhs.y) - static_cast<Tres>(rhs)
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator*(const basic_point<T>& lhs, U rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_point<Tres>{
            static_cast<Tres>(lhs.x) * static_cast<Tres>(rhs),
            static_cast<Tres>(lhs.y) * static_cast<Tres>(rhs)
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator/(const basic_point<T>& lhs, U rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_point<Tres>{
            static_cast<Tres>(lhs.x) / static_cast<Tres>(rhs),
            static_cast<Tres>(lhs.y) / static_cast<Tres>(rhs)
        };
    }
    /// @}
    
    /**
     * @name Global Binary Operators (Scalar vs Point)
     * Operations where the scalar is on the left-hand side.
     * @{
     */

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator+(U lhs, const basic_point<T>& rhs) noexcept {
        return rhs + lhs; 
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator*(U lhs, const basic_point<T>& rhs) noexcept {
        return rhs * lhs; 
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator-(U lhs, const basic_point<T>& rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_point<Tres>{
            static_cast<Tres>(lhs) - static_cast<Tres>(rhs.x),
            static_cast<Tres>(lhs) - static_cast<Tres>(rhs.y)
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator/(U lhs, const basic_point<T>& rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_point<Tres>{
            static_cast<Tres>(lhs) / static_cast<Tres>(rhs.x),
            static_cast<Tres>(lhs) / static_cast<Tres>(rhs.y)
        };
    }
    /// @}

    /// @brief Type alias for a point with integer coordinates.
    using Point = basic_point<int>;

    /// @brief Type alias for a point with float coordinates.
    using Pointf = basic_point<float>;

} // namespace zuu::widget
