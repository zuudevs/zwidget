#pragma once

/**
 * @file window.hpp
 * @brief Core window implementation with Win32 backend
 * @version 1.0
 * @date 2025-11-29
 */

#include "zwidget/unit/size.hpp"
#include "zwidget/unit/point.hpp"
#include "zwidget/unit/color.hpp"
#include "zwidget/unit/event.hpp"
#include "zwidget/core/event_dispatcher.hpp"
#include <Windows.h>
#include <Windowsx.h>
#include <string>
#include <functional>

#undef min
#undef max

namespace zuu::widget {

/**
 * @brief Window creation parameters
 */
struct WindowConfig {
    std::wstring title = L"ZWidget Window";
    Size size = {800, 600};
    Point position = {100, 100};
    Color background = Color::white();
    bool resizable = true;
    bool decorated = true;
    bool visible = true;
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD ex_style = 0;
};

/**
 * @brief Main window class - RAII wrapper around HWND
 */
class Window {
public:
    using EventCallback = std::function<void(const Event&)>;

private:
    HWND hwnd_ = nullptr;
    HINSTANCE instance_ = nullptr;
    WindowConfig config_;
    EventCallback event_callback_;
    EventDispatcher dispatcher_;
    bool should_close_ = false;
    
    static constexpr const wchar_t* WINDOW_CLASS_NAME = L"ZWidgetWindowClass";
    static inline bool class_registered_ = false;

    // Win32 window procedure
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    
    // Register window class (called once)
    static void RegisterWindowClass(HINSTANCE instance);
    
    // Process individual messages
    LRESULT HandleMessage(UINT msg, WPARAM wp, LPARAM lp);

public:
    Window() = default;
    
    /**
     * @brief Create window with configuration
     */
    explicit Window(const WindowConfig& config);
    
    /**
     * @brief Destructor - automatically destroys window
     */
    ~Window();
    
    // Non-copyable
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    
    // Movable
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;
    
    /**
     * @brief Create and show window
     */
    bool create(const WindowConfig& config);
    
    /**
     * @brief Destroy window
     */
    void destroy();
    
    /**
     * @brief Process all pending events (non-blocking)
     */
    void poll_events();
    
    /**
     * @brief Wait for events (blocking)
     */
    void wait_events();
    
    /**
     * @brief Set event callback handler
     */
    void set_event_callback(EventCallback callback) {
        event_callback_ = std::move(callback);
    }
    
    /**
     * @brief Get event dispatcher
     */
    EventDispatcher& dispatcher() { return dispatcher_; }
    const EventDispatcher& dispatcher() const { return dispatcher_; }
    
    /**
     * @brief Check if window should close
     */
    bool should_close() const noexcept { return should_close_; }
    
    /**
     * @brief Set close flag
     */
    void set_should_close(bool value) noexcept { should_close_ = value; }
    
    /**
     * @brief Get native window handle
     */
    HWND native_handle() const noexcept { return hwnd_; }
    
    /**
     * @brief Check if window is valid
     */
    bool is_valid() const noexcept { return hwnd_ != nullptr; }
    
    /**
     * @brief Show/hide window
     */
    void set_visible(bool visible);
    
    /**
     * @brief Get window size
     */
    Size get_size() const;
    
    /**
     * @brief Set window size
     */
    void set_size(const Size& size);
    
    /**
     * @brief Get window position
     */
    Point get_position() const;
    
    /**
     * @brief Set window position
     */
    void set_position(const Point& pos);
    
    /**
     * @brief Set window title
     */
    void set_title(const std::wstring& title);
    
    /**
     * @brief Get window title
     */
    std::wstring get_title() const;
    
    /**
     * @brief Minimize window
     */
    void minimize();
    
    /**
     * @brief Maximize window
     */
    void maximize();
    
    /**
     * @brief Restore window
     */
    void restore();
    
    /**
     * @brief Focus window
     */
    void focus();
    
    /**
     * @brief Get client area size
     */
    Size get_client_size() const;
};

} // namespace zuu::widget