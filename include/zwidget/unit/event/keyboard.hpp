#pragma once

/**
 * @file keyboard.hpp
 * @brief Keyboard event structures
 * @version 1.0
 * @date 2025-11-29
 */

#include "keymod.hpp"
#include <cstdint>

namespace zuu::widget {

enum class keyboard_state : uint8_t {
    none,
    press,
    release,
    char_input  // For text input
};

class KeyboardEvent {
public:
    uint32_t key_code = 0;         // 4 bytes - Virtual key code (VK_*)
    uint32_t scan_code = 0;        // 4 bytes - Hardware scan code
    wchar_t character = L'\0';     // 2 bytes - For char_input events
    keyboard_state state = keyboard_state::none;  // 1 byte
    key_modifier modifiers = key_modifier::none;  // 1 byte
    bool is_repeat = false;        // 1 byte - Key repeat flag

private:
    uint8_t _padding[3] = {};      // Explicit padding for alignment

public:
    constexpr KeyboardEvent() noexcept = default;
    
    constexpr KeyboardEvent(keyboard_state s, uint32_t key) noexcept
        : key_code(key), state(s) {}
    
    constexpr KeyboardEvent(
        keyboard_state s, 
        uint32_t key, 
        key_modifier mods
    ) noexcept
        : key_code(key), state(s), modifiers(mods) {}
    
    constexpr KeyboardEvent(
        keyboard_state s, 
        uint32_t key, 
        wchar_t ch,
        key_modifier mods = key_modifier::none,
        bool repeat = false
    ) noexcept
        : key_code(key)
        , character(ch)
        , state(s)
        , modifiers(mods)
        , is_repeat(repeat) {}
    
    // Comparison
    constexpr bool operator==(const KeyboardEvent&) const noexcept = default;
};

} // namespace zuu::widget
