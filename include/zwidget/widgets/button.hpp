#pragma once

/**
 * @file button.hpp
 * @brief Button widget with hover and press states
 * @version 1.0
 * @date 2025-11-30
 */

#include "zwidget/core/widget.hpp"
#include "zwidget/render/canvas.hpp"
#include <functional>

namespace zuu::widget {

/**
 * @brief Button widget - clickable button with visual feedback
 */
class Button : public Widget {
public:
    using ClickCallback = std::function<void()>;
    
private:
    std::wstring text_;
    TextStyle text_style_;
    
    float border_radius_ = 4.0f;
    float border_width_ = 1.0f;
    
    Color normal_bg_;
    Color hover_bg_;
    Color pressed_bg_;
    Color disabled_bg_;
    
    Color border_color_{128, 128, 128, 255};
    
    ClickCallback on_click_;
    
public:
    Button() {
        set_preferred_size(Sizef{120, 40});
        set_background(Color::white());
        set_foreground(Color::black());
        
        // Default colors
        normal_bg_ = Color::white();
        hover_bg_ = Color(230, 230, 230, 255);
        pressed_bg_ = Color(200, 200, 200, 255);
        disabled_bg_ = Color(240, 240, 240, 255);
        
        text_style_.font_size = 14.0f;
        text_style_.align = TextAlign::center;
        text_style_.valign = TextVAlign::middle;
    }
    
    explicit Button(const std::wstring& text) : Button() {
        text_ = text;
    }
    
    Button(const std::wstring& text, const TextStyle& style) : Button(text) {
        text_style_ = style;
    }
    
    // === Text Management ===
    
    void set_text(const std::wstring& text) {
        if (text_ != text) {
            text_ = text;
            mark_dirty();
        }
    }
    
    const std::wstring& text() const { return text_; }
    
    void set_text_style(const TextStyle& style) {
        text_style_ = style;
        mark_dirty();
    }
    
    const TextStyle& text_style() const { return text_style_; }
    
    void set_font_size(float size) {
        text_style_.font_size = size;
        mark_dirty();
    }
    
    void set_bold(bool bold) {
        text_style_.bold = bold;
        mark_dirty();
    }
    
    // === Appearance ===
    
    void set_border_radius(float radius) {
        border_radius_ = radius;
        mark_dirty();
    }
    
    float border_radius() const { return border_radius_; }
    
    void set_border_width(float width) {
        border_width_ = width;
        mark_dirty();
    }
    
    float border_width() const { return border_width_; }
    
    void set_border_color(const Color& color) {
        border_color_ = color;
        mark_dirty();
    }
    
    const Color& border_color() const { return border_color_; }
    
    // === State Colors ===
    
    void set_normal_color(const Color& color) {
        normal_bg_ = color;
        set_background(color);
        mark_dirty();
    }
    
    void set_hover_color(const Color& color) {
        hover_bg_ = color;
        mark_dirty();
    }
    
    void set_pressed_color(const Color& color) {
        pressed_bg_ = color;
        mark_dirty();
    }
    
    void set_disabled_color(const Color& color) {
        disabled_bg_ = color;
        mark_dirty();
    }
    
    // === Callbacks ===
    
    void set_on_click(ClickCallback callback) {
        on_click_ = std::move(callback);
    }
    
    // === Widget Interface ===
    
    void draw(Canvas& canvas) override {
        Rectf rect{0, 0, width(), height()};
        
        // Determine background color based on state
        Color bg_color;
        if (!is_enabled()) {
            bg_color = disabled_bg_;
        } else if (is_pressed()) {
            bg_color = pressed_bg_;
        } else if (is_hovered()) {
            bg_color = hover_bg_;
        } else {
            bg_color = normal_bg_;
        }
        
        // Draw background
        if (border_radius_ > 0) {
            canvas.fill_rounded_rect(rect, border_radius_, bg_color);
        } else {
            canvas.fill_rect(rect, bg_color);
        }
        
        // Draw border
        if (border_width_ > 0) {
            Color current_border = is_focused() ? 
                Color(0, 120, 215, 255) : border_color_;
            
            if (border_radius_ > 0) {
                canvas.draw_rounded_rect(rect, border_radius_, current_border, border_width_);
            } else {
                canvas.draw_rect(rect, current_border, border_width_);
            }
        }
        
        // Draw text
        if (!text_.empty()) {
            Color text_color = is_enabled() ? foreground() : Color(150, 150, 150, 255);
            canvas.draw_text(text_, rect, text_color, text_style_);
        }
    }
    
    bool on_mouse_enter() override {
        set_state(WidgetState::hovered, true);
        mark_dirty();
        return false;
    }
    
    bool on_mouse_leave() override {
        set_state(WidgetState::hovered, false);
        set_state(WidgetState::pressed, false);
        mark_dirty();
        return false;
    }
    
    bool on_mouse_press(mouse_button button, const Pointf& pos) override {
        if (button == mouse_button::left && is_enabled()) {
            set_state(WidgetState::pressed, true);
            mark_dirty();
            return true;
        }
        return false;
    }
    
    bool on_mouse_release(mouse_button button, const Pointf& pos) override {
        if (button == mouse_button::left && is_enabled()) {
            bool was_pressed = is_pressed();
            set_state(WidgetState::pressed, false);
            mark_dirty();
            
            // Trigger click if released while still inside button
            if (was_pressed && contains(pos)) {
                if (on_click_) {
                    on_click_();
                }
            }
            
            return true;
        }
        return false;
    }
    
    bool on_key_press(uint32_t key) override {
        if ((key == VK_RETURN || key == VK_SPACE) && is_enabled()) {
            if (on_click_) {
                on_click_();
            }
            return true;
        }
        return false;
    }
};

/**
 * @brief Helper to create button
 */
inline WidgetPtr make_button(const std::wstring& text) {
    return std::make_shared<Button>(text);
}

inline WidgetPtr make_button(const std::wstring& text, Button::ClickCallback callback) {
    auto button = std::make_shared<Button>(text);
    button->set_on_click(std::move(callback));
    return button;
}

/**
 * @brief Icon button - button with icon support (future enhancement)
 */
class IconButton : public Button {
private:
    // Icon placeholder - will be implemented when image loading is ready
    std::wstring icon_path_;
    
public:
    IconButton() : Button() {
        set_preferred_size(Sizef{40, 40});
    }
    
    explicit IconButton(const std::wstring& icon) : IconButton() {
        icon_path_ = icon;
    }
    
    void set_icon(const std::wstring& icon) {
        icon_path_ = icon;
        mark_dirty();
    }
    
    const std::wstring& icon() const { return icon_path_; }
    
    void draw(Canvas& canvas) override {
        // For now, just draw as regular button
        Button::draw(canvas);
        
        // TODO: Draw icon when image loading is implemented
        // if (!icon_path_.empty()) {
        //     // Draw icon image
        // }
    }
};

/**
 * @brief Toggle button - button that stays pressed
 */
class ToggleButton : public Button {
public:
    using ToggledCallback = std::function<void(bool)>;
    
private:
    bool toggled_ = false;
    ToggledCallback on_toggled_;
    
public:
    ToggleButton() : Button() {}
    
    explicit ToggleButton(const std::wstring& text) : Button(text) {}
    
    void set_toggled(bool toggled) {
        if (toggled_ != toggled) {
            toggled_ = toggled;
            set_state(WidgetState::pressed, toggled);
            mark_dirty();
            
            if (on_toggled_) {
                on_toggled_(toggled_);
            }
        }
    }
    
    bool is_toggled() const { return toggled_; }
    
    void toggle() {
        set_toggled(!toggled_);
    }
    
    void set_on_toggled(ToggledCallback callback) {
        on_toggled_ = std::move(callback);
    }
    
    bool on_mouse_release(mouse_button button, const Pointf& pos) override {
        if (button == mouse_button::left && is_enabled() && contains(pos)) {
            toggle();
            return true;
        }
        return false;
    }
    
    bool on_key_press(uint32_t key) override {
        if ((key == VK_RETURN || key == VK_SPACE) && is_enabled()) {
            toggle();
            return true;
        }
        return false;
    }
};

/**
 * @brief Helper to create toggle button
 */
inline WidgetPtr make_toggle_button(const std::wstring& text, bool initial = false) {
    auto button = std::make_shared<ToggleButton>(text);
    button->set_toggled(initial);
    return button;
}

} // namespace zuu::widget