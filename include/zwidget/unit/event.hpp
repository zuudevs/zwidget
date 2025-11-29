#pragma once

/**
 * @file event.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Type-safe event system dengan HWND encapsulation (std::variant version)
 * @version 2.1.0
 * @date 2025-11-29
 * 
 * @details Event system yang:
 * - Menyembunyikan HWND dari user (security)
 * - Menggunakan std::variant untuk type-safe storage
 * - Auto type deduction dengan std::get dan std::visit
 * - Zero-cost abstraction dengan modern C++
 */


#include "event/window.hpp"
#include "event/mouse.hpp"
#include "event/keyboard.hpp"
#include <variant>

namespace zuu::widget {

enum class event_type : uint8_t {
    none,
    window,
    mouse,
    keyboard
};

struct EmptyEvent {
    constexpr EmptyEvent() noexcept = default;
    constexpr bool operator==(const EmptyEvent&) const noexcept = default;
};

class Event {
public :
	using event_variant = std::variant<
        EmptyEvent,
        WindowEvent,
        MouseEvent,
        KeyboardEvent> ;
private:
    event_variant data_ ;
    HWND handle_ {} ;
    event_type type_ = event_type::none ;

public:
    // ============= Constructors =============
    
    constexpr Event() noexcept : data_(EmptyEvent{}) {}
    
    constexpr Event(const Event&) noexcept = default;
    constexpr Event(Event&&) noexcept = default;
    constexpr Event& operator=(const Event&) noexcept = default;
    constexpr Event& operator=(Event&&) noexcept = default;
    
    // Internal constructors (used by EventDispatcher)
    constexpr Event(WindowEvent evt, HWND h = nullptr) noexcept 
        : data_(evt), handle_(h), type_(event_type::window) {}
    
    constexpr Event(MouseEvent evt, HWND h = nullptr) noexcept 
        : data_(evt), handle_(h), type_(event_type::mouse) {}
    
    constexpr Event(KeyboardEvent evt, HWND h = nullptr) noexcept 
        : data_(evt), handle_(h), type_(event_type::keyboard) {}

    // ============= Type Queries =============
    
    [[nodiscard]] constexpr event_type type() const noexcept { 
        return type_; 
    }
    
    [[nodiscard]] constexpr bool is_window() const noexcept { 
        return type_ == event_type::window; 
    }
    
    [[nodiscard]] constexpr bool is_mouse() const noexcept { 
        return type_ == event_type::mouse; 
    }
    
    [[nodiscard]] constexpr bool is_keyboard() const noexcept { 
        return type_ == event_type::keyboard; 
    }
    
    [[nodiscard]] constexpr bool is_none() const noexcept { 
        return type_ == event_type::none; 
    }
    
    /**
     * @brief Check apakah event menyimpan tipe T
     * @tparam T Tipe event data
     */
    template <typename T>
    [[nodiscard]] constexpr bool holds() const noexcept {
        return std::holds_alternative<T>(data_);
    }
    
    /**
     * @brief Get variant index
     */
    [[nodiscard]] constexpr std::size_t index() const noexcept {
        return data_.index();
    }

    // ============= Handle Access =============
    
    [[nodiscard]] constexpr const EventHandle& handle() const noexcept { 
        return handle_; 
    }

    // ============= Data Access - std::get Style =============
    
    /**
     * @brief Get reference ke event data (std::get style)
     * @tparam T Tipe event data (WindowEvent, MouseEvent, KeyboardEvent)
     * @return Reference ke event data
     * @throws std::bad_variant_access jika tipe salah
     * 
     * @example
     * auto& mouse = event.get<MouseEvent>();
     * auto& window = std::get<WindowEvent>(event);  // juga bisa!
     */
    template <typename T>
    [[nodiscard]] constexpr T& get() & {
        return std::get<T>(data_);
    }
    
    template <typename T>
    [[nodiscard]] constexpr const T& get() const& {
        return std::get<T>(data_);
    }
    
    template <typename T>
    [[nodiscard]] constexpr T&& get() && {
        return std::get<T>(std::move(data_));
    }
    
    template <typename T>
    [[nodiscard]] constexpr const T&& get() const&& {
        return std::get<T>(std::move(data_));
    }

    // ============= Data Access - By Index =============
    
    /**
     * @brief Get by index (std::variant style)
     * @tparam I Index (0=EmptyEvent, 1=WindowEvent, 2=MouseEvent, 3=KeyboardEvent)
     */
    template <std::size_t I>
    [[nodiscard]] constexpr auto& get() & {
        return std::get<I>(data_);
    }
    
    template <std::size_t I>
    [[nodiscard]] constexpr const auto& get() const& {
        return std::get<I>(data_);
    }
    
    template <std::size_t I>
    [[nodiscard]] constexpr auto&& get() && {
        return std::get<I>(std::move(data_));
    }
    
    template <std::size_t I>
    [[nodiscard]] constexpr const auto&& get() const&& {
        return std::get<I>(std::move(data_));
    }

    // ============= Safe Pointer Access =============
    
    /**
     * @brief Get pointer (nullptr jika tipe salah) - std::get_if style
     * @tparam T Tipe event data
     * @return Pointer atau nullptr (safe, no throw)
     * 
     * @example
     * if (auto* mouse = event.get_if<MouseEvent>()) {
     *     // Use mouse safely
     * }
     */
    template <typename T>
    [[nodiscard]] constexpr T* get_if() noexcept {
        return std::get_if<T>(&data_);
    }
    
    template <typename T>
    [[nodiscard]] constexpr const T* get_if() const noexcept {
        return std::get_if<T>(&data_);
    }

    // ============= Visitation (std::visit Style) =============
    
    /**
     * @brief Visit pattern untuk type-safe dispatch (std::visit style)
     * @param visitor Callable dengan overload untuk setiap tipe event
     * 
     * @example
     * std::visit([](auto&& arg) {
     *     using T = std::decay_t<decltype(arg)>;
     *     if constexpr (std::is_same_v<T, MouseEvent>) {
     *         // Handle mouse
     *     }
     * }, event);
     * 
     * // Atau dengan overload helper:
     * event.visit(overload{
     *     [](const WindowEvent& e) { ... },
     *     [](const MouseEvent& e) { ... },
     *     [](const KeyboardEvent& e) { ... },
     *     [](const EmptyEvent&) { ... }
     * });
     */
    template <typename Visitor>
    constexpr decltype(auto) visit(Visitor&& visitor) & {
        return std::visit(std::forward<Visitor>(visitor), data_);
    }
    
    template <typename Visitor>
    constexpr decltype(auto) visit(Visitor&& visitor) const& {
        return std::visit(std::forward<Visitor>(visitor), data_);
    }
    
    template <typename Visitor>
    constexpr decltype(auto) visit(Visitor&& visitor) && {
        return std::visit(std::forward<Visitor>(visitor), std::move(data_));
    }
    
    template <typename Visitor>
    constexpr decltype(auto) visit(Visitor&& visitor) const&& {
        return std::visit(std::forward<Visitor>(visitor), std::move(data_));
    }

    // ============= Direct Variant Access =============
    
    /**
     * @brief Get underlying variant
     * @note Advanced use only - untuk compatibility dengan std::variant functions
     */
    [[nodiscard]] constexpr event_variant& variant() & noexcept { 
        return data_; 
    }
    
    [[nodiscard]] constexpr const event_variant& variant() const& noexcept { 
        return data_; 
    }
    
    [[nodiscard]] constexpr event_variant&& variant() && noexcept { 
        return std::move(data_); 
    }
    
    [[nodiscard]] constexpr const event_variant&& variant() const&& noexcept { 
        return std::move(data_); 
    }

    // ============= Comparison =============
    
    [[nodiscard]] constexpr bool operator==(const Event& o) const noexcept {
        return type_ == o.type_ && data_ == o.data_;
    }
};

// ============= std::variant ADL Support =============

/**
 * @brief std::get overload untuk Event (ADL support)
 * Memungkinkan std::get<T>(event) syntax
 */
template <typename T>
[[nodiscard]] constexpr T& get(Event& e) {
    return e.get<T>();
}

template <typename T>
[[nodiscard]] constexpr const T& get(const Event& e) {
    return e.get<T>();
}

template <typename T>
[[nodiscard]] constexpr T&& get(Event&& e) {
    return std::move(e).get<T>();
}

template <typename T>
[[nodiscard]] constexpr const T&& get(const Event&& e) {
    return std::move(e).get<T>();
}

/**
 * @brief std::get_if overload untuk Event (ADL support)
 */
template <typename T>
[[nodiscard]] constexpr T* get_if(Event* e) noexcept {
    return e ? e->get_if<T>() : nullptr;
}

template <typename T>
[[nodiscard]] constexpr const T* get_if(const Event* e) noexcept {
    return e ? e->get_if<T>() : nullptr;
}

/**
 * @brief std::visit overload untuk Event (ADL support)
 */
template <typename Visitor>
constexpr decltype(auto) visit(Visitor&& visitor, Event& e) {
    return e.visit(std::forward<Visitor>(visitor));
}

template <typename Visitor>
constexpr decltype(auto) visit(Visitor&& visitor, const Event& e) {
    return e.visit(std::forward<Visitor>(visitor));
}

template <typename Visitor>
constexpr decltype(auto) visit(Visitor&& visitor, Event&& e) {
    return std::move(e).visit(std::forward<Visitor>(visitor));
}

template <typename Visitor>
constexpr decltype(auto) visit(Visitor&& visitor, const Event&& e) {
    return std::move(e).visit(std::forward<Visitor>(visitor));
}

/**
 * @brief std::holds_alternative overload untuk Event
 */
template <typename T>
[[nodiscard]] constexpr bool holds_alternative(const Event& e) noexcept {
    return e.holds<T>();
}

// ============= Overload Helper (std::visit compatible) =============

/**
 * @brief Helper untuk visitor overload pattern
 * Compatible dengan std::visit
 * 
 * @example
 * event.visit(overload{
 *     [](int i) { ... },
 *     [](double d) { ... }
 * });
 */
template <typename... Fs>
struct overload : Fs... {
    using Fs::operator()...;
};

template <typename... Fs>
overload(Fs...) -> overload<Fs...>;

// ============= Helper Functions =============

/**
 * @brief Factory functions untuk pembuatan event
 * @details Simplifikasi pembuatan event tanpa constructor langsung
 */

[[nodiscard]] inline constexpr Event make_window_event(
    event_state::window state,
    HWND handle = nullptr
) noexcept {
    return Event(WindowEvent{state}, handle);
}

[[nodiscard]] inline constexpr Event make_window_event(
    event_state::window state,
    basic_size<int> size,
    HWND handle = nullptr
) noexcept {
    return Event(WindowEvent{state, size}, handle);
}

[[nodiscard]] inline constexpr Event make_mouse_event(
    event_state::mouse state,
    basic_point<int> pos,
    HWND handle = nullptr
) noexcept {
    return Event(MouseEvent{state, pos}, handle);
}

[[nodiscard]] inline constexpr Event make_mouse_event(
    event_state::mouse state,
    event_state::mouse_button button,
    basic_point<int> pos,
    HWND handle = nullptr
) noexcept {
    return Event(MouseEvent{state, button, pos}, handle);
}

[[nodiscard]] inline constexpr Event make_keyboard_event(
    event_state::keyboard state,
    uint32_t key_code,
    HWND handle = nullptr
) noexcept {
    return Event(KeyboardEvent{state, key_code}, handle);
}

} // namespace zuu::widget

// ============= std::variant_size and std::variant_alternative Support =============

namespace std {
    // Expose Event as variant-like type untuk generic programming
    template <>
    struct variant_size<zuu::widget::Event> 
        : std::integral_constant<std::size_t, 4> {};
    
    template <std::size_t I>
    struct variant_alternative<I, zuu::widget::Event> {
        using type = std::variant_alternative_t<I, typename zuu::widget::Event::event_variant>;
    };
}
