#pragma once

/**
 * @file event.hpp
 * @author zuudevs (zuudevs@gmail.com)
 * @brief Type-safe event system dengan HWND encapsulation
 * @version 2.1.0
 * @date 2025-11-29
 */

#include "event/window.hpp"
#include "event/mouse.hpp"
#include "event/keyboard.hpp"
#include <variant>
#include <Windows.h>

namespace zuu::widget {

// Type alias untuk HWND (bisa diubah untuk cross-platform nanti)
using EventHandle = HWND;

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
public:
    using event_variant = std::variant<
        EmptyEvent,
        WindowEvent,
        MouseEvent,
        KeyboardEvent>;
        
private:
    event_variant data_;
    EventHandle handle_ = nullptr;
    event_type type_ = event_type::none;

public:
    // ============= Constructors =============
    
    constexpr Event() noexcept : data_(EmptyEvent{}) {}
    
    constexpr Event(const Event&) noexcept = default;
    constexpr Event(Event&&) noexcept = default;
    constexpr Event& operator=(const Event&) noexcept = default;
    constexpr Event& operator=(Event&&) noexcept = default;
    
    // Internal constructors (used by EventDispatcher)
    constexpr Event(WindowEvent evt, EventHandle h = nullptr) noexcept 
        : data_(evt), handle_(h), type_(event_type::window) {}
    
    constexpr Event(MouseEvent evt, EventHandle h = nullptr) noexcept 
        : data_(evt), handle_(h), type_(event_type::mouse) {}
    
    constexpr Event(KeyboardEvent evt, EventHandle h = nullptr) noexcept 
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
    
    [[nodiscard]] constexpr EventHandle handle() const noexcept { 
        return handle_; 
    }

    // ============= Data Access - std::get Style =============
    
    /**
     * @brief Get reference ke event data (std::get style)
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
     * @brief Visit pattern untuk type-safe dispatch
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

template <typename T>
[[nodiscard]] constexpr T* get_if(Event* e) noexcept {
    return e ? e->get_if<T>() : nullptr;
}

template <typename T>
[[nodiscard]] constexpr const T* get_if(const Event* e) noexcept {
    return e ? e->get_if<T>() : nullptr;
}

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

template <typename T>
[[nodiscard]] constexpr bool holds_alternative(const Event& e) noexcept {
    return e.holds<T>();
}

// ============= Overload Helper =============

template <typename... Fs>
struct overload : Fs... {
    using Fs::operator()...;
};

template <typename... Fs>
overload(Fs...) -> overload<Fs...>;

// ============= Helper Functions =============

[[nodiscard]] inline constexpr Event make_window_event(
    window_state state,
    EventHandle handle = nullptr
) noexcept {
    return Event(WindowEvent{state}, handle);
}

[[nodiscard]] inline constexpr Event make_window_event(
    window_state state,
    Size size,
    EventHandle handle = nullptr
) noexcept {
    return Event(WindowEvent{state, size}, handle);
}

[[nodiscard]] inline constexpr Event make_mouse_event(
    mouse_state state,
    Pointf pos,
    EventHandle handle = nullptr
) noexcept {
    return Event(MouseEvent{state, pos}, handle);
}

[[nodiscard]] inline constexpr Event make_mouse_event(
    mouse_state state,
    mouse_button button,
    Pointf pos,
    EventHandle handle = nullptr
) noexcept {
    return Event(MouseEvent{state, button, pos}, handle);
}

[[nodiscard]] inline constexpr Event make_keyboard_event(
    keyboard_state state,
    uint32_t key_code,
    EventHandle handle = nullptr
) noexcept {
    return Event(KeyboardEvent{state, key_code}, handle);
}

} // namespace zuu::widget

// ============= std::variant_size and std::variant_alternative Support =============

namespace std {
    template <>
    struct variant_size<zuu::widget::Event> 
        : std::integral_constant<std::size_t, 4> {};
    
    template <std::size_t I>
    struct variant_alternative<I, zuu::widget::Event> {
        using type = std::variant_alternative_t<I, typename zuu::widget::Event::event_variant>;
    };
} // namespace std
