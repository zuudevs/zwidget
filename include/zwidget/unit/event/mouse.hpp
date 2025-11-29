#pragma once

/**
 * @file mouse.hpp
 * @brief Mouse event structures
 * @version 1.0
 * @date 2025-11-29
 */

#include "keymod.hpp"
#include "zwidget/unit/point.hpp"

namespace zuu::widget {

enum class mouse_state : uint8_t {
    none,
    move,
    enter,
    leave,
    press,
    release,
    double_click,
    scroll
};

enum class mouse_button : uint8_t {
    none,
    left,
    middle,
    right,
    x1,     // Additional mouse button 1
    x2      // Additional mouse button 2
};

class MouseEvent {
public:
    Pointf position{};                          // 8 bytes (2x float)
    int scroll_delta = 0;                       // 4 bytes
    mouse_state state = mouse_state::none;      // 1 byte
    mouse_button button = mouse_button::none;   // 1 byte
    key_modifier modifiers = key_modifier::none; // 1 byte

private:
    uint8_t _padding = 0;  // Explicit padding for alignment

public:
    constexpr MouseEvent() noexcept = default;
    
    constexpr MouseEvent(mouse_state s, const Pointf& pos) noexcept
        : position(pos), state(s) {}
    
    constexpr MouseEvent(
        mouse_state s, 
        mouse_button btn, 
        const Pointf& pos
    ) noexcept 
        : position(pos), state(s), button(btn) {}
    
    constexpr MouseEvent(
        mouse_state s,
        const Pointf& pos,
        key_modifier mods
    ) noexcept
        : position(pos), state(s), modifiers(mods) {}
    
    constexpr MouseEvent(
        mouse_state s,
        mouse_button btn,
        const Pointf& pos,
        key_modifier mods
    ) noexcept
        : position(pos), state(s), button(btn), modifiers(mods) {}
    
    // For scroll events
    constexpr MouseEvent(
        const Pointf& pos,
        int delta,
        key_modifier mods = key_modifier::none
    ) noexcept
        : position(pos)
        , scroll_delta(delta)
        , state(mouse_state::scroll)
        , modifiers(mods) {}
    
    // Comparison
    constexpr bool operator==(const MouseEvent&) const noexcept = default;
};

} // namespace zuu::widget
