#pragma once

/**
 * @file size.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Defines the basic_size class and internal compatibility traits.
 * @version 1.1
 * @date 2025-11-29
 * * @details This header provides a generic 2D size structure representing Width and Height.
 * It includes internal meta-programming traits to ensure integral sizes are stored 
 * as unsigned types to prevent negative dimensions, with saturated arithmetic (clamping)
 * on subtraction to prevent underflow.
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
     * @brief A template class representing a 2D size with Width and Height.
     * 
	 * @tparam Base The underlying numeric type for the dimensions. 
     * Must satisfy the `_meta::Numeric` concept.
     * Note: If Base is integral, it will be stored as unsigned internally to ensure non-negative dimensions.
     */
    template <_meta::Numeric Base>
    class basic_size {
    public :
        /// @brief The underlying value type (potentially modified by size_compat).
        using base_type = _meta::size_compat_t<Base>;

        _meta::size_compat_t<Base> w {}; ///< The Width component.
        _meta::size_compat_t<Base> h {}; ///< The Height component.

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
         * @brief Constructs a size with specific width and height.
         * 
		 * @tparam Tw Type of the width.
         * @tparam Th Type of the height.
         * @param _w The value for width.
         * @param _h The value for height.
         */
        template <_meta::Numeric Tw, _meta::Numeric Th>
        constexpr basic_size(Tw _w, Th _h) noexcept
         : w(static_cast<base_type>(_w)), h(static_cast<base_type>(_h)) {}

        /**
         * @brief Explicit constructor from a single scalar value.
         * Initializes both width and height with the same value (square size).
         * * @tparam T The type of the scalar value.
         * @param val The value to assign to both w and h.
         */
        template <_meta::Numeric T>
        explicit constexpr basic_size(T val) noexcept
         : w(static_cast<base_type>(val)), h(static_cast<base_type>(val)) {}

        /// @}

        /**
         * @brief Checks if the size is empty (both dimensions are zero).
         * @return true if w == 0 and h == 0.
         */
        constexpr bool empty() const noexcept {
            return w == 0 && h == 0 ;
        }

        /**
         * @brief Checks if any dimension is zero.
         * @return true if w == 0 or h == 0.
         */
        constexpr bool has_zero() const noexcept {
            return w == 0 || h == 0 ;
        }

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
            return basic_size<T>{static_cast<T>(w), static_cast<T>(h)};
        }

        /**
         * @brief Component-wise addition assignment.
         */
        template <_meta::Numeric T>
        constexpr basic_size& operator+=(const basic_size<T>& rhs) noexcept {
            w += static_cast<base_type>(rhs.w);
            h += static_cast<base_type>(rhs.h);
            return *this;
        }

        /**
         * @brief Component-wise subtraction assignment with Clamping.
         * @details Clamps the result to 0 if the subtraction would result in a negative value (underflow).
         */
        template <_meta::Numeric T>
        constexpr basic_size& operator-=(const basic_size<T>& rhs) noexcept {
            auto rw = static_cast<base_type>(rhs.w);
            auto rh = static_cast<base_type>(rhs.h);

            w = (w < rw) ? 0 : w - rw;
            h = (h < rh) ? 0 : h - rh;
            
            return *this;
        }

        /**
         * @brief Component-wise multiplication assignment.
         */
        template <_meta::Numeric T>
        constexpr basic_size& operator*=(const basic_size<T>& rhs) noexcept {
            w *= static_cast<base_type>(rhs.w);
            h *= static_cast<base_type>(rhs.h);
            return *this;
        }

        /**
         * @brief Component-wise division assignment.
         */
        template <_meta::Numeric T>
        constexpr basic_size& operator/=(const basic_size<T>& rhs) noexcept {
            w /= static_cast<base_type>(rhs.w);
            h /= static_cast<base_type>(rhs.h);
            return *this;
        }

        // --- Scalar assignment operators ---

        /**
         * @brief Adds a scalar value to both dimensions.
         */
        template <_meta::Numeric U>
        constexpr basic_size& operator+=(U val) noexcept {
            w += static_cast<base_type>(val);
            h += static_cast<base_type>(val);
            return *this;
        }

        /**
         * @brief Subtracts a scalar value from both dimensions with Clamping.
         * @details Clamps result to 0 to prevent underflow.
         */
        template <_meta::Numeric U>
        constexpr basic_size& operator-=(U val) noexcept {
            auto v = static_cast<base_type>(val);

            w = (w < v) ? 0 : w - v;
            h = (h < v) ? 0 : h - v;
            
            return *this;
        }

        /**
         * @brief Multiplies both dimensions by a scalar value.
         */
        template <_meta::Numeric U>
        constexpr basic_size& operator*=(U val) noexcept {
            w *= static_cast<base_type>(val);
            h *= static_cast<base_type>(val);
            return *this;
        }

        /**
         * @brief Divides both dimensions by a scalar value.
         */
        template <_meta::Numeric U>
        constexpr basic_size& operator/=(U val) noexcept {
            w /= static_cast<base_type>(val);
            h /= static_cast<base_type>(val);
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
            static_cast<Tres>(a.w) + static_cast<Tres>(b.w),
            static_cast<Tres>(a.h) + static_cast<Tres>(b.h)
        };
    }

    template <_meta::Numeric Tlhs, _meta::Numeric Trhs>
    constexpr auto operator-(const basic_size<Tlhs>& a, const basic_size<Trhs>& b) noexcept {
        using Tres = std::common_type_t
        <typename basic_size<Tlhs>::base_type
        , typename basic_size<Trhs>::base_type>;

        auto aw = static_cast<Tres>(a.w);
        auto ah = static_cast<Tres>(a.h);
        auto bw = static_cast<Tres>(b.w);
        auto bh = static_cast<Tres>(b.h);

        return basic_size<Tres>{
            (aw < bw) ? 0 : aw - bw,
            (ah < bh) ? 0 : ah - bh 
        };
    }

    template <_meta::Numeric Tlhs, _meta::Numeric Trhs>
    constexpr auto operator*(const basic_size<Tlhs>& a, const basic_size<Trhs>& b) noexcept {
        using Tres = std::common_type_t
        <typename basic_size<Tlhs>::base_type
        , typename basic_size<Trhs>::base_type>;
        
        return basic_size<Tres>{
            static_cast<Tres>(a.w) * static_cast<Tres>(b.w),
            static_cast<Tres>(a.h) * static_cast<Tres>(b.h)
        };
    }

    template <_meta::Numeric Tlhs, _meta::Numeric Trhs>
    constexpr auto operator/(const basic_size<Tlhs>& a, const basic_size<Trhs>& b) noexcept {
        using Tres = std::common_type_t
        <typename basic_size<Tlhs>::base_type
        , typename basic_size<Trhs>::base_type>;

        return basic_size<Tres>{
            static_cast<Tres>(a.w) / static_cast<Tres>(b.w),
            static_cast<Tres>(a.h) / static_cast<Tres>(b.h)
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
            static_cast<Tres>(lhs.w) + static_cast<Tres>(rhs),
            static_cast<Tres>(lhs.h) + static_cast<Tres>(rhs)
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator-(const basic_size<T>& lhs, U rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        
        auto lw = static_cast<Tres>(lhs.w);
        auto lh = static_cast<Tres>(lhs.h);
        auto r  = static_cast<Tres>(rhs);

        return basic_size<Tres>{
            (lw < r) ? 0 : lw - r,
            (lh < r) ? 0 : lh - r
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator*(const basic_size<T>& lhs, U rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_size<Tres>{
            static_cast<Tres>(lhs.w) * static_cast<Tres>(rhs),
            static_cast<Tres>(lhs.h) * static_cast<Tres>(rhs)
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator/(const basic_size<T>& lhs, U rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_size<Tres>{
            static_cast<Tres>(lhs.w) / static_cast<Tres>(rhs),
            static_cast<Tres>(lhs.h) / static_cast<Tres>(rhs)
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

        auto l  = static_cast<Tres>(lhs);
        auto rw = static_cast<Tres>(rhs.w);
        auto rh = static_cast<Tres>(rhs.h);

        return basic_size<Tres>{
            (l < rw) ? 0 : l - rw,
            (l < rh) ? 0 : l - rh
        };
    }

    template <_meta::Numeric T, _meta::Numeric U>
    constexpr auto operator/(U lhs, const basic_size<T>& rhs) noexcept {
        using Tres = std::common_type_t<T, U>;
        return basic_size<Tres>{
            static_cast<Tres>(lhs) / static_cast<Tres>(rhs.w),
            static_cast<Tres>(lhs) / static_cast<Tres>(rhs.h)
        };
    }
    /// @}

    /// @brief Type alias for a size with integer coordinates (unsigned internally).
    using Size = basic_size<int>;

    /// @brief Type alias for a size with float coordinates.
    using Sizef = basic_size<float>;

} // namespace zuu::widget
