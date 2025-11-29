#include "zwidget/core/window.hpp"
#include <stdexcept>

namespace zuu::widget {

// Static window procedure
LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Window* window = nullptr;
    
    if (msg == WM_NCCREATE) {
        // Store window pointer during creation
        auto* create_struct = reinterpret_cast<CREATESTRUCT*>(lp);
        window = static_cast<Window*>(create_struct->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        window->hwnd_ = hwnd;
    } else {
        // Retrieve window pointer
        window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    if (window) {
        return window->HandleMessage(msg, wp, lp);
    }
    
    return DefWindowProc(hwnd, msg, wp, lp);
}

void Window::RegisterWindowClass(HINSTANCE instance) {
    if (class_registered_) return;
    
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    
    if (!RegisterClassExW(&wc)) {
        throw std::runtime_error("Failed to register window class");
    }
    
    class_registered_ = true;
}

LRESULT Window::HandleMessage(UINT msg, WPARAM wp, LPARAM lp) {
    Event event;
    
    switch (msg) {
        case WM_CLOSE:
            event = make_window_event(window_state::close, hwnd_);
            should_close_ = true;
            break;
            
        case WM_DESTROY:
            event = make_window_event(window_state::quit, hwnd_);
            PostQuitMessage(0);
            break;
            
        case WM_SIZE: {
            int w = LOWORD(lp);
            int h = HIWORD(lp);
            
            window_state state = window_state::resize;
            if (wp == SIZE_MINIMIZED) state = window_state::minimize;
            else if (wp == SIZE_MAXIMIZED) state = window_state::maximize;
            else if (wp == SIZE_RESTORED) state = window_state::restore;
            
            event = make_window_event(state, Size{w, h}, hwnd_);
            break;
        }
        
        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lp);
            int y = GET_Y_LPARAM(lp);
            event = make_mouse_event(mouse_state::move, Pointf{float(x), float(y)}, hwnd_);
            break;
        }
        
        case WM_LBUTTONDOWN:
            event = make_mouse_event(
                mouse_state::press,
                mouse_button::left,
                Pointf{float(GET_X_LPARAM(lp)), float(GET_Y_LPARAM(lp))},
                hwnd_
            );
            break;
            
        case WM_LBUTTONUP:
            event = make_mouse_event(
                mouse_state::release,
                mouse_button::left,
                Pointf{float(GET_X_LPARAM(lp)), float(GET_Y_LPARAM(lp))},
                hwnd_
            );
            break;
            
        case WM_RBUTTONDOWN:
            event = make_mouse_event(
                mouse_state::press,
                mouse_button::right,
                Pointf{float(GET_X_LPARAM(lp)), float(GET_Y_LPARAM(lp))},
                hwnd_
            );
            break;
            
        case WM_RBUTTONUP:
            event = make_mouse_event(
                mouse_state::release,
                mouse_button::right,
                Pointf{float(GET_X_LPARAM(lp)), float(GET_Y_LPARAM(lp))},
                hwnd_
            );
            break;
            
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            event = make_keyboard_event(
                keyboard_state::press,
                static_cast<uint32_t>(wp),
                hwnd_
            );
            break;
            
        case WM_KEYUP:
        case WM_SYSKEYUP:
            event = make_keyboard_event(
                keyboard_state::release,
                static_cast<uint32_t>(wp),
                hwnd_
            );
            break;
            
        case WM_SETFOCUS:
            event = make_window_event(window_state::focus_gained, hwnd_);
            break;
            
        case WM_KILLFOCUS:
            event = make_window_event(window_state::focus_lost, hwnd_);
            break;
            
        default:
            return DefWindowProc(hwnd_, msg, wp, lp);
    }
    
    // Dispatch event to callback
    if (event_callback_ && !event.is_none()) {
        event_callback_(event);
    }
    
    return 0;
}

Window::Window(const WindowConfig& config) {
    create(config);
}

Window::~Window() {
    destroy();
}

Window::Window(Window&& other) noexcept
    : hwnd_(other.hwnd_)
    , instance_(other.instance_)
    , config_(std::move(other.config_))
    , event_callback_(std::move(other.event_callback_))
    , should_close_(other.should_close_)
{
    other.hwnd_ = nullptr;
    other.instance_ = nullptr;
    other.should_close_ = false;
}

Window& Window::operator=(Window&& other) noexcept {
    if (this != &other) {
        destroy();
        
        hwnd_ = other.hwnd_;
        instance_ = other.instance_;
        config_ = std::move(other.config_);
        event_callback_ = std::move(other.event_callback_);
        should_close_ = other.should_close_;
        
        other.hwnd_ = nullptr;
        other.instance_ = nullptr;
        other.should_close_ = false;
    }
    return *this;
}

bool Window::create(const WindowConfig& config) {
    if (hwnd_) {
        destroy();
    }
    
    config_ = config;
    instance_ = GetModuleHandle(nullptr);
    
    RegisterWindowClass(instance_);
    
    // Adjust window size for client area
    RECT rect = {0, 0, static_cast<LONG>(config.size.w), static_cast<LONG>(config.size.h)};
    AdjustWindowRectEx(&rect, config.style, FALSE, config.ex_style);
    
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    hwnd_ = CreateWindowExW(
        config.ex_style,
        WINDOW_CLASS_NAME,
        config.title.c_str(),
        config.style,
        config.position.x,
        config.position.y,
        width,
        height,
        nullptr,
        nullptr,
        instance_,
        this  // Pass 'this' pointer
    );
    
    if (!hwnd_) {
        return false;
    }
    
    if (config.visible) {
        ShowWindow(hwnd_, SW_SHOW);
        UpdateWindow(hwnd_);
    }
    
    return true;
}

void Window::destroy() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

void Window::poll_events() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void Window::wait_events() {
    MSG msg;
    if (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void Window::set_visible(bool visible) {
    if (hwnd_) {
        ShowWindow(hwnd_, visible ? SW_SHOW : SW_HIDE);
    }
}

Size Window::get_size() const {
    if (!hwnd_) return {};
    
    RECT rect;
    GetWindowRect(hwnd_, &rect);
    return Size{rect.right - rect.left, rect.bottom - rect.top};
}

void Window::set_size(const Size& size) {
    if (hwnd_) {
        SetWindowPos(hwnd_, nullptr, 0, 0, size.w, size.h, 
                     SWP_NOMOVE | SWP_NOZORDER);
    }
}

Point Window::get_position() const {
    if (!hwnd_) return {};
    
    RECT rect;
    GetWindowRect(hwnd_, &rect);
    return Point{rect.left, rect.top};
}

void Window::set_position(const Point& pos) {
    if (hwnd_) {
        SetWindowPos(hwnd_, nullptr, pos.x, pos.y, 0, 0, 
                     SWP_NOSIZE | SWP_NOZORDER);
    }
}

void Window::set_title(const std::wstring& title) {
    if (hwnd_) {
        SetWindowTextW(hwnd_, title.c_str());
        config_.title = title;
    }
}

std::wstring Window::get_title() const {
    if (!hwnd_) return config_.title;
    
    int len = GetWindowTextLengthW(hwnd_);
    if (len == 0) return L"";
    
    std::wstring title(len + 1, L'\0');
    GetWindowTextW(hwnd_, title.data(), len + 1);
    title.resize(len);
    return title;
}

void Window::minimize() {
    if (hwnd_) {
        ShowWindow(hwnd_, SW_MINIMIZE);
    }
}

void Window::maximize() {
    if (hwnd_) {
        ShowWindow(hwnd_, SW_MAXIMIZE);
    }
}

void Window::restore() {
    if (hwnd_) {
        ShowWindow(hwnd_, SW_RESTORE);
    }
}

void Window::focus() {
    if (hwnd_) {
        SetForegroundWindow(hwnd_);
        SetFocus(hwnd_);
    }
}

Size Window::get_client_size() const {
    if (!hwnd_) return {};
    
    RECT rect;
    GetClientRect(hwnd_, &rect);
    return Size{rect.right - rect.left, rect.bottom - rect.top};
}

} // namespace zuu::widget