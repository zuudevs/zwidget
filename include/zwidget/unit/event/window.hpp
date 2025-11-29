#pragma once

/**
 * @file window.hpp
 * @brief Window event structures
 * @version 1.0
 * @date 2025-11-29
 */

#include "zwidget/unit/size.hpp"

namespace zuu::widget {

enum class window_state : uint8_t {
    none,
    quit,
    close,
    minimize,
    maximize,
    resize,
    restore,
    hide,
    show,
    focus_gained,
    focus_lost,
    moved
};

class WindowEvent {
public:
    Size size{};                              // 8 bytes (2x uint32_t)
    window_state state = window_state::none;  // 1 byte

private:
    uint8_t _padding[3] = {};  // Explicit padding for alignment

public:
    constexpr WindowEvent() noexcept = default;
    
    constexpr WindowEvent(window_state s) noexcept 
        : state(s) {}
    
    constexpr WindowEvent(window_state s, const Size& size_) noexcept
        : size(size_), state(s) {}
    
    // Comparison
    constexpr bool operator==(const WindowEvent&) const noexcept = default;
};

} // namespace zuu::widget
