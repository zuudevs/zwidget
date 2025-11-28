#pragma once

/**
 * @file size.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Defines the basic_size class and internal compatibility traits.
 * @version 1.0
 * @date 2025-11-28
 * 
 * @details This header provides a generic 2D size structure (width, height).
 * It includes internal meta-programming traits to handle type compatibility,
 * specifically ensuring that integral sizes are treated as unsigned types.
 */

#include <concepts>
#include "zwidget/concept/numeric.hpp"

namespace zuu::_meta {

    /**
     * @brief Primary template for size compatibility traits.
     * Used to determine the underlying storage type for size classes.
     * @tparam T The numeric type to check.
     */
    template <typename T>
    struct size_compat;

    /**
     * @brief Specialization for integral types.
     * Forces integers to be unsigned when used in size contexts 
     * (e.g., int becomes unsigned int).
     */
    template <std::integral T>
    struct size_compat<T>
     : std::type_identity<std::make_unsigned_t<T>> {};

    /**
     * @brief Specialization for floating-point types.
     * Keeps floating point types as is (e.g., float remains float).
     */
    template <std::floating_point T>
    struct size_compat<T>
     : std::type_identity<T> {};

    /**
     * @brief Helper alias type to access the compatible size type.
     */
    template <typename T>
    using size_compat_t = typename size_compat<T>::type;

} // namespace zuu::_meta

namespace zuu::widget {

    /**
     * @brief A template class representing a 2D size with (x, y) coordinates.
     * 
	 * @tparam Base The underlying numeric type for the coordinates. 
     * Must satisfy the `_meta::Numeric` concept.
     * Note: If Base is integral, it will be stored as unsigned internally.
     */
    template <_meta::Numeric Base>
    class basic_size {
    public :
        /// @brief The underlying value type (potentially modified by size_compat).
        using base_type = _meta::size_compat_t<Base>;

        _meta::size_compat_t<Base> x {}; ///< The X coordinate (Width).
        _meta::size_compat_t<Base> y {}; ///< The Y coordinate (Height).

        /// @name Constructors & Destructor
        /// @{

        constexpr basic_size() noexcept = default;
        constexpr basic_size(const basic_size&) noexcept = default;
        constexpr basic_size(basic_size&&) noexcept = default;
        constexpr basic_size& operator=(const basic_size&) noexcept = default;
        constexpr basic_size& operator=(basic_size&&) noexcept = default;
        
        /// @brief Three-way comparison operator (spaceship operator).
        constexpr auto operator<=>(const basic_size&) const noexcept = default;
        
        constexpr ~basic_size() noexcept = default;

        /**
         * @brief Constructs a size with specific x (width) and y (height).
         * 
		 * @tparam Tx Type of the x coordinate.
         * @tparam Ty Type of the y coordinate.
         * @param _x The value for the x coordinate.
         * @param _y The value for the y coordinate.
         */
        template <_meta::Numeric Tx, _meta::Numeric Ty>
        constexpr basic_size(Tx _x, Ty _y) noexcept
         : x(static_cast<base_type>(_x)), y(static_cast<base_type>(_y)) {}

        /**
         * @brief Explicit constructor from a single scalar value.
         * Initializes both x and y with the same value.
         * 
		 * @tparam T The type of the scalar value.
         * @param val The value to assign to both x and y.
         */
        template <_meta::Numeric T>
        explicit constexpr basic_size(T val) noexcept
         : x(static_cast<base_type>(val)), y(static_cast<base_type>(val)) {}

        /// @}

        /// @name Conversions & Compound Assignment
        /// @{

        /**
         * @brief Type conversion operator to a basic_size of a different type.
         * 
		 * @tparam T The target numeric type.
         * @return A new basic_size cast to type T.
         */
        template <_meta::Numeric T>
        constexpr operator basic_size<T>() const noexcept {
            return basic_size<T>{static_cast<T>(x), static_cast<T>(y)};
        }

        /**
         * @brief Component-wise addition assignment.
         */
        template <_meta::Numeric T>
        constexpr basic_size& operator+=(const basic_size<T>& rhs) noexcept {
            x += static_cast<base_type>(rhs.x);
            y += static_cast<base_type>(rhs.y);
            return *this;
        }

        /**
         * @brief Component-wise subtraction assignment.
         */
        template <_meta::Numeric T>
        constexpr basic_size& operator-=(const basic_size<T>& rhs) noexcept {
            x -= static_cast<base_type>(rhs.x);
            y -= static_cast<base_type>(rhs.y);
            return *this;
        }

        /**
         * @brief Component-wise multiplication assignment.
         */
        template <_meta::Numeric T>
        constexpr basic_size& operator*=(const basic_size<T>& rhs) noexcept {
            x *= static_cast<base_type>(rhs.x);
            y *= static_cast<base_type>(rhs.y);
            return *this;
        }

        /**
         * @brief Component-wise division assignment.
         */
        template <_meta::Numeric T>
        constexpr basic_size& operator/=(const basic_size<T>& rhs) noexcept {
            x /= static_cast<base_type>(rhs.x);
            y /= static_cast<base_type>(rhs.y);
            return *this;
        }

        // --- Scalar assignment operators ---

        /**
         * @brief Adds a scalar value to both coordinates.
         */
        template <_meta::Numeric U>
        constexpr basic_size& operator+=(U val) noexcept {
            x += static_cast<base_type>(val);
            y += static_cast<base_type>(val);
            return *this;
        }

        /**
         * @brief Subtracts a scalar value from both coordinates.
         */
        template <_meta::Numeric U>
        constexpr basic_size& operator-=(U val) noexcept {
            x -= static_cast<base_type>(val);
            y -= static_cast<base_type>(val);
            return *this;
        }

        /**
         * @brief Multiplies both coordinates by a scalar value.
         */
        template <_meta::Numeric U>
        constexpr basic_size& operator*=(U val) noexcept {
            x *= static_cast<base_type>(val);
            y *= static_cast<base_type>(val);
            return *this;
        }

        /**
         * @brief Divides both coordinates by a scalar value.
         */
        template <_meta::Numeric U>
        constexpr basic_size& operator/=(U val) noexcept {
            x /= static_cast<base_type>(val);
            y /= static_cast<base_type>(val);
            return *this;
        }
        /// @}
    };

    /**
     * @name Global Binary Operators (Size vs Size)
     * Operations between two sizes. The result type is deduced using `std::common_type` on the base types.
     * @{
     */

    template <_meta::Numeric Tlhs, _meta::Numeric Trhs>
    constexpr auto operator+(const basic_size<Tlhs>& a, const basic_size<Trhs>& b) noexcept {
        using Tres = std::common_type_t
        <typename basic_size<Tlhs>::base_type
        , typename basic_size<Trhs>::base_type>;

        return basic_size<Tres>{
            static_cast<Tres>(a.x) + static_cast<Tres>(b.x),
            static_cast<Tres>(a.y) + static_cast<Tres>(b.y)
        };
    }

    template <_meta::Numeric Tlhs, _meta::Numeric Trhs>
    constexpr auto operator-(const basic_size<Tlhs>& a, const basic_size<Trhs>& b) noexcept {
        using Tres = std::common_type_t
        <typename basic_size<Tlhs>::base_type
        , typename basic_size<Trhs>::base_type>;

        return basic_size<Tres>{
            static_cast<Tres>(a.x) - static_cast<Tres>(b.x),
            static_cast<Tres>(a.y) - static_cast<Tres>(b.y)
        };
    }

    template <_meta::Numeric Tlhs, _meta::Numeric Trhs>
    constexpr auto operator*(const basic_size<Tlhs>& a, const basic_size<Trhs>& b) noexcept {
        using Tres = std::common_type_t
        <typename basic_size<Tlhs>::base_type
        , typename basic_size<Trhs>::base_type>;
        
        return basic_size<Tres>{
            static_cast<Tres>(a.x) * static_cast<Tres>(b.x),
            static_cast<Tres>(a.y) * static_cast<Tres>(b.y)
        };
    }

    template <_meta::Numeric Tlhs, _meta::Numeric Trhs>
    constexpr auto operator/(const basic_size<Tlhs>& a, const basic_size<Trhs>& b) noexcept {
        using Tres = std::common_type_t
        <typename basic_size<Tlhs>::base_type
        , typename basic_size<Trhs>::base_type>;

        return basic_size<Tres>{
            static_cast<Tres>(a.x) / static_cast<Tres>(b.x),
            static_cast<Tres>(a.y) / static_cast<Tres>(b.y)
        };
    }
    /// @}

    /**
     * @name Global Binary Operators (Size vs Scalar)
     * Operations between a size and a scalar.
     * @{
     */

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator+(const basic_size<T>& lhs, U rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_size<Tres>{
            static_cast<Tres>(lhs.x) + static_cast<Tres>(rhs),
            static_cast<Tres>(lhs.y) + static_cast<Tres>(rhs)
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator-(const basic_size<T>& lhs, U rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_size<Tres>{
            static_cast<Tres>(lhs.x) - static_cast<Tres>(rhs),
            static_cast<Tres>(lhs.y) - static_cast<Tres>(rhs)
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator*(const basic_size<T>& lhs, U rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_size<Tres>{
            static_cast<Tres>(lhs.x) * static_cast<Tres>(rhs),
            static_cast<Tres>(lhs.y) * static_cast<Tres>(rhs)
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator/(const basic_size<T>& lhs, U rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_size<Tres>{
            static_cast<Tres>(lhs.x) / static_cast<Tres>(rhs),
            static_cast<Tres>(lhs.y) / static_cast<Tres>(rhs)
        };
    }
    /// @}
    
    /**
     * @name Global Binary Operators (Scalar vs Size)
     * Operations where the scalar is on the left-hand side.
     * @{
     */

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator+(U lhs, const basic_size<T>& rhs) noexcept {
        return rhs + lhs; 
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator*(U lhs, const basic_size<T>& rhs) noexcept {
        return rhs * lhs; 
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator-(U lhs, const basic_size<T>& rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_size<Tres>{
            static_cast<Tres>(lhs) - static_cast<Tres>(rhs.x),
            static_cast<Tres>(lhs) - static_cast<Tres>(rhs.y)
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator/(U lhs, const basic_size<T>& rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_size<Tres>{
            static_cast<Tres>(lhs) / static_cast<Tres>(rhs.x),
            static_cast<Tres>(lhs) / static_cast<Tres>(rhs.y)
        };
    }
    /// @}

    /// @brief Type alias for a size with integer coordinates.
    using Size = basic_size<int>;

    /// @brief Type alias for a size with float coordinates.
    using Sizef = basic_size<float>;

} // namespace zuu::widget
