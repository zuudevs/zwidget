#pragma once

/**
 * @file checkbox.hpp
 * @brief Checkbox widget with label
 * @version 1.0
 * @date 2025-11-30
 */

#include "zwidget/core/widget.hpp"
#include "zwidget/render/canvas.hpp"
#include <functional>

namespace zuu::widget {

/**
 * @brief Checkbox widget for boolean input
 */
class CheckBox : public Widget {
public:
    using CheckedChangedCallback = std::function<void(bool)>;
    
private:
    std::wstring label_;
    bool checked_ = false;
    bool tristate_ = false;
    int state_ = 0; // 0 = unchecked, 1 = checked, 2 = indeterminate
    
    TextStyle label_style_;
    float box_size_ = 16.0f;
    float spacing_ = 8.0f;
    
    CheckedChangedCallback on_checked_changed_;
    
public:
    CheckBox() {
        set_preferred_size(Sizef{100, 24});
        set_background(Color::transparent());
        set_foreground(Color::black());
        
        label_style_.font_size = 12.0f;
        label_style_.align = TextAlign::left;
        label_style_.valign = TextVAlign::middle;
    }
    
    explicit CheckBox(const std::wstring& label) : CheckBox() {
        label_ = label;
    }
    
    CheckBox(const std::wstring& label, bool checked) : CheckBox(label) {
        checked_ = checked;
        state_ = checked ? 1 : 0;
    }
    
    // === State Management ===
    
    void set_checked(bool checked) {
        if (checked_ != checked) {
            checked_ = checked;
            state_ = checked ? 1 : 0;
            mark_dirty();
            
            if (on_checked_changed_) {
                on_checked_changed_(checked_);
            }
        }
    }
    
    bool is_checked() const { return checked_; }
    
    void toggle() {
        set_checked(!checked_);
    }
    
    void set_tristate(bool tristate) {
        tristate_ = tristate;
    }
    
    bool is_tristate() const { return tristate_; }
    
    void set_state(int state) {
        if (tristate_) {
            state_ = std::clamp(state, 0, 2);
            checked_ = (state_ == 1);
            mark_dirty();
            
            if (on_checked_changed_) {
                on_checked_changed_(checked_);
            }
        }
    }
    
    int state() const { return state_; }
    
    // === Label ===
    
    void set_label(const std::wstring& label) {
        if (label_ != label) {
            label_ = label;
            mark_dirty();
        }
    }
    
    const std::wstring& label() const { return label_; }
    
    void set_label_style(const TextStyle& style) {
        label_style_ = style;
        mark_dirty();
    }
    
    const TextStyle& label_style() const { return label_style_; }
    
    // === Callbacks ===
    
    void set_on_checked_changed(CheckedChangedCallback callback) {
        on_checked_changed_ = std::move(callback);
    }
    
    // === Appearance ===
    
    void set_box_size(float size) {
        box_size_ = size;
        mark_dirty();
    }
    
    float box_size() const { return box_size_; }
    
    // === Widget Interface ===
    
    void draw(Canvas& canvas) override {
        float y_center = height() / 2.0f;
        
        // Draw checkbox box
        Rectf box_rect{
            0.0f, 
            y_center - box_size_ / 2.0f,
            box_size_,
            box_size_
        };
        
        // Background
        Color box_bg = is_hovered() ? Color(240, 240, 240, 255) : Color::white();
        canvas.fill_rect(box_rect, box_bg);
        
        // Border
        Color border_color = is_focused() ? 
            Color(0, 120, 215, 255) : Color(128, 128, 128, 255);
        canvas.draw_rect(box_rect, border_color, 1.0f);
        
        // Check mark or indeterminate
        if (state_ == 1) {
            // Draw checkmark
            float padding = box_size_ * 0.25f;
            Rectf inner = box_rect;
            inner.pos.x += padding;
            inner.pos.y += padding;
            inner.size.w -= padding * 2;
            inner.size.h -= padding * 2;
            
            // Simple checkmark using lines
            Pointf p1{inner.pos.x, inner.pos.y + inner.size.h * 0.5f};
            Pointf p2{inner.pos.x + inner.size.w * 0.4f, inner.pos.y + inner.size.h * 0.8f};
            Pointf p3{inner.pos.x + inner.size.w, inner.pos.y + inner.size.h * 0.2f};
            
            canvas.draw_line(p1, p2, Color(0, 120, 215, 255), 2.0f);
            canvas.draw_line(p2, p3, Color(0, 120, 215, 255), 2.0f);
        } else if (state_ == 2) {
            // Draw indeterminate (dash)
            float padding = box_size_ * 0.3f;
            Rectf dash{
                box_rect.pos.x + padding,
                y_center - 1.0f,
                box_size_ - padding * 2,
                2.0f
            };
            canvas.fill_rect(dash, Color(128, 128, 128, 255));
        }
        
        // Draw label
        if (!label_.empty()) {
            Rectf label_rect{
                box_size_ + spacing_,
                0.0f,
                width() - box_size_ - spacing_,
                height()
            };
            
            Color text_color = is_enabled() ? foreground() : Color(128, 128, 128, 255);
            canvas.draw_text(label_, label_rect, text_color, label_style_);
        }
    }
    
    bool on_mouse_press(mouse_button button, const Pointf& pos) override {
        if (button == mouse_button::left && is_enabled()) {
            if (tristate_) {
                state_ = (state_ + 1) % 3;
                checked_ = (state_ == 1);
            } else {
                toggle();
            }
            
            mark_dirty();
            return true;
        }
        return false;
    }
    
    bool on_key_press(uint32_t key) override {
        if (key == VK_SPACE && is_enabled()) {
            if (tristate_) {
                state_ = (state_ + 1) % 3;
                checked_ = (state_ == 1);
            } else {
                toggle();
            }
            
            mark_dirty();
            
            if (on_checked_changed_) {
                on_checked_changed_(checked_);
            }
            
            return true;
        }
        return false;
    }
};

/**
 * @brief Helper to create checkbox
 */
inline WidgetPtr make_checkbox(const std::wstring& label, bool checked = false) {
    return std::make_shared<CheckBox>(label, checked);
}

} // namespace zuu::widget
