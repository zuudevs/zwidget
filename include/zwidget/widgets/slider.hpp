#pragma once

/**
 * @file slider.hpp
 * @brief Horizontal slider widget for numeric input
 * @version 1.0
 * @date 2025-11-30
 */

#include "zwidget/core/widget.hpp"
#include "zwidget/render/canvas.hpp"
#include <functional>
#include <algorithm>

namespace zuu::widget {

/**
 * @brief Slider widget for selecting numeric values
 */
class Slider : public Widget {
public:
    using ValueChangedCallback = std::function<void(float)>;
    
private:
    float value_ = 0.0f;
    float min_value_ = 0.0f;
    float max_value_ = 100.0f;
    float step_ = 1.0f;
    
    bool dragging_ = false;
    float track_height_ = 4.0f;
    float thumb_radius_ = 10.0f;
    
    Color track_color_{200, 200, 200, 255};
    Color track_fill_color_{0, 120, 215, 255};
    Color thumb_color_{0, 120, 215, 255};
    Color thumb_hover_color_{0, 100, 180, 255};
    
    ValueChangedCallback on_value_changed_;
    
    // Helper methods
    float get_normalized_value() const {
        return (value_ - min_value_) / (max_value_ - min_value_);
    }
    
    float get_thumb_x() const {
        float normalized = get_normalized_value();
        float track_width = width() - thumb_radius_ * 2;
        return thumb_radius_ + normalized * track_width;
    }
    
    void set_value_from_position(float x) {
        float track_width = width() - thumb_radius_ * 2;
        float normalized = std::clamp((x - thumb_radius_) / track_width, 0.0f, 1.0f);
        
        float new_value = min_value_ + normalized * (max_value_ - min_value_);
        
        // Apply step
        if (step_ > 0) {
            new_value = std::round(new_value / step_) * step_;
        }
        
        set_value(new_value);
    }
    
public:
    Slider() {
        set_preferred_size(Sizef{200, 30});
        set_background(Color::transparent());
    }
    
    Slider(float min_val, float max_val, float initial = 0.0f) : Slider() {
        min_value_ = min_val;
        max_value_ = max_val;
        value_ = std::clamp(initial, min_val, max_val);
    }
    
    // === Value Management ===
    
    void set_value(float value) {
        float clamped = std::clamp(value, min_value_, max_value_);
        
        if (value_ != clamped) {
            value_ = clamped;
            mark_dirty();
            
            if (on_value_changed_) {
                on_value_changed_(value_);
            }
        }
    }
    
    float value() const { return value_; }
    
    void set_range(float min_val, float max_val) {
        min_value_ = min_val;
        max_value_ = max_val;
        value_ = std::clamp(value_, min_val, max_val);
        mark_dirty();
    }
    
    float min_value() const { return min_value_; }
    float max_value() const { return max_value_; }
    
    void set_step(float step) {
        step_ = step;
    }
    
    float step() const { return step_; }
    
    // === Appearance ===
    
    void set_track_height(float height) {
        track_height_ = height;
        mark_dirty();
    }
    
    void set_thumb_radius(float radius) {
        thumb_radius_ = radius;
        mark_dirty();
    }
    
    void set_colors(const Color& track, const Color& fill, const Color& thumb) {
        track_color_ = track;
        track_fill_color_ = fill;
        thumb_color_ = thumb;
        mark_dirty();
    }
    
    // === Callbacks ===
    
    void set_on_value_changed(ValueChangedCallback callback) {
        on_value_changed_ = std::move(callback);
    }
    
    // === Widget Interface ===
    
    void draw(Canvas& canvas) override {
        float y_center = height() / 2.0f;
        float thumb_x = get_thumb_x();
        
        // Draw track background
        Rectf track_bg{
            thumb_radius_,
            y_center - track_height_ / 2.0f,
            width() - thumb_radius_ * 2,
            track_height_
        };
        canvas.fill_rounded_rect(track_bg, track_height_ / 2.0f, track_color_);
        
        // Draw filled portion
        float fill_width = thumb_x - thumb_radius_;
        if (fill_width > 0) {
            Rectf track_fill{
                thumb_radius_,
                y_center - track_height_ / 2.0f,
                fill_width,
                track_height_
            };
            canvas.fill_rounded_rect(track_fill, track_height_ / 2.0f, track_fill_color_);
        }
        
        // Draw thumb
        Color current_thumb_color = (is_hovered() || dragging_) ? 
            thumb_hover_color_ : thumb_color_;
        
        canvas.fill_circle(
            Pointf{thumb_x, y_center},
            thumb_radius_,
            current_thumb_color
        );
        
        // Draw thumb border
        canvas.draw_circle(
            Pointf{thumb_x, y_center},
            thumb_radius_,
            Color::white(),
            2.0f
        );
        
        // Optional: Draw value text
        if (is_hovered() || dragging_) {
            TextStyle style;
            style.font_size = 10.0f;
            style.align = TextAlign::center;
            
            std::wstring value_text = std::to_wstring(static_cast<int>(value_));
            Sizef text_size = canvas.measure_text(value_text, style);
            
            Rectf value_rect{
                thumb_x - text_size.w / 2.0f,
                y_center - thumb_radius_ - text_size.h - 5,
                text_size.w,
                text_size.h
            };
            
            canvas.draw_text(value_text, value_rect, Color::black(), style);
        }
    }
    
    bool on_mouse_press(mouse_button button, const Pointf& pos) override {
        if (button == mouse_button::left && is_enabled()) {
            dragging_ = true;
            set_value_from_position(pos.x);
            mark_dirty();
            return true;
        }
        return false;
    }
    
    bool on_mouse_release(mouse_button button, const Pointf& pos) override {
        if (button == mouse_button::left) {
            dragging_ = false;
            mark_dirty();
            return true;
        }
        return false;
    }
    
    bool on_mouse_move(const Pointf& pos) override {
        if (dragging_) {
            set_value_from_position(pos.x);
            return true;
        }
        return false;
    }
    
    bool on_key_press(uint32_t key) override {
        if (!is_enabled()) return false;
        
        float change = step_ > 0 ? step_ : (max_value_ - min_value_) / 10.0f;
        
        switch (key) {
            case VK_LEFT:
            case VK_DOWN:
                set_value(value_ - change);
                return true;
                
            case VK_RIGHT:
            case VK_UP:
                set_value(value_ + change);
                return true;
                
            case VK_HOME:
                set_value(min_value_);
                return true;
                
            case VK_END:
                set_value(max_value_);
                return true;
        }
        
        return false;
    }
};

/**
 * @brief Helper to create slider
 */
inline WidgetPtr make_slider(float min_val, float max_val, float initial = 0.0f) {
    return std::make_shared<Slider>(min_val, max_val, initial);
}

} // namespace zuu::widget