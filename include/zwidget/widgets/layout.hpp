#pragma once

/**
 * @file layout.hpp
 * @brief Layout containers: HBox (horizontal) and VBox (vertical)
 * @version 1.0
 * @date 2025-11-30
 */

#include "zwidget/render/canvas.hpp"
#include "zwidget/core/widget.hpp"
#include <algorithm>

namespace zuu::widget {

/**
 * @brief Layout alignment options
 */
enum class LayoutAlign {
    start,    // Top for VBox, Left for HBox
    center,
    end,      // Bottom for VBox, Right for HBox
    stretch   // Fill available space
};

/**
 * @brief Base layout container
 */
class LayoutContainer : public Widget {
protected:
    float spacing_ = 0.0f;
    float padding_ = 0.0f;
    LayoutAlign alignment_ = LayoutAlign::start;
    
public:
    LayoutContainer() {
        set_background(Color::transparent());
    }
    
    void set_spacing(float spacing) {
        spacing_ = spacing;
        mark_dirty();
    }
    
    float spacing() const { return spacing_; }
    
    void set_padding(float padding) {
        padding_ = padding;
        mark_dirty();
    }
    
    float padding() const { return padding_; }
    
    void set_alignment(LayoutAlign align) {
        alignment_ = align;
        mark_dirty();
    }
    
    LayoutAlign alignment() const { return alignment_; }
    
    void draw(Canvas& canvas) override {
        // Draw background if not transparent
        if (background().a > 0) {
            canvas.fill_rect(Rectf{0, 0, width(), height()}, background());
        }
        
        // Children will be drawn by base Widget::render
    }
};

/**
 * @brief Horizontal box layout - arranges children left to right
 */
class HBox : public LayoutContainer {
public:
    HBox() = default;
    
    void layout() override {
        if (children().empty()) return;
        
        // Calculate total preferred width and count stretch items
        float total_preferred_width = 0.0f;
        size_t stretch_count = 0;
        
        for (const auto& child : children()) {
            if (!child->is_visible()) continue;
            
            if (child->alignment().orientation == orientations::horizontal &&
                child->alignment().main_axis == aligns::end) {
                stretch_count++;
            } else {
                total_preferred_width += child->preferred_size().w;
            }
        }
        
        // Add spacing
        size_t visible_count = 0;
        for (const auto& child : children()) {
            if (child->is_visible()) visible_count++;
        }
        
        float total_spacing = spacing_ * std::max(0, static_cast<int>(visible_count) - 1);
        float available_width = width() - padding_ * 2 - total_spacing - total_preferred_width;
        
        // Calculate stretch width
        float stretch_width = stretch_count > 0 ? available_width / stretch_count : 0.0f;
        
        // Position children
        float x = padding_;
        
        for (auto& child : children_) {
            if (!child->is_visible()) continue;
            
            float child_width;
            bool is_stretch = (child->alignment().orientation == orientations::horizontal &&
                             child->alignment().main_axis == aligns::end);
            
            if (is_stretch) {
                child_width = std::max(stretch_width, child->min_size().w);
            } else {
                child_width = child->preferred_size().w;
            }
            
            // Clamp to min/max
            child_width = std::clamp(child_width, 
                                    child->min_size().w, 
                                    child->max_size().w);
            
            // Calculate vertical position based on alignment
            float y = padding_;
            float child_height = height() - padding_ * 2;
            
            switch (alignment_) {
                case LayoutAlign::start:
                    y = padding_;
                    child_height = child->preferred_size().h;
                    break;
                    
                case LayoutAlign::center:
                    child_height = child->preferred_size().h;
                    y = (height() - child_height) / 2.0f;
                    break;
                    
                case LayoutAlign::end:
                    child_height = child->preferred_size().h;
                    y = height() - padding_ - child_height;
                    break;
                    
                case LayoutAlign::stretch:
                    y = padding_;
                    child_height = height() - padding_ * 2;
                    break;
            }
            
            // Set child bounds
            child->set_bounds(Rectf{x, y, child_width, child_height});
            
            // Layout child's children
            child->layout();
            
            x += child_width + spacing_;
        }
    }
};

/**
 * @brief Vertical box layout - arranges children top to bottom
 */
class VBox : public LayoutContainer {
public:
    VBox() = default;
    
    void layout() override {
        if (children().empty()) return;
        
        // Calculate total preferred height and count stretch items
        float total_preferred_height = 0.0f;
        size_t stretch_count = 0;
        
        for (const auto& child : children()) {
            if (!child->is_visible()) continue;
            
            if (child->alignment().orientation == orientations::vertical &&
                child->alignment().main_axis == aligns::end) {
                stretch_count++;
            } else {
                total_preferred_height += child->preferred_size().h;
            }
        }
        
        // Add spacing
        size_t visible_count = 0;
        for (const auto& child : children()) {
            if (child->is_visible()) visible_count++;
        }
        
        float total_spacing = spacing_ * std::max(0, static_cast<int>(visible_count) - 1);
        float available_height = height() - padding_ * 2 - total_spacing - total_preferred_height;
        
        // Calculate stretch height
        float stretch_height = stretch_count > 0 ? available_height / stretch_count : 0.0f;
        
        // Position children
        float y = padding_;
        
        for (auto& child : children_) {
            if (!child->is_visible()) continue;
            
            float child_height;
            bool is_stretch = (child->alignment().orientation == orientations::vertical &&
                             child->alignment().main_axis == aligns::end);
            
            if (is_stretch) {
                child_height = std::max(stretch_height, child->min_size().h);
            } else {
                child_height = child->preferred_size().h;
            }
            
            // Clamp to min/max
            child_height = std::clamp(child_height,
                                     child->min_size().h,
                                     child->max_size().h);
            
            // Calculate horizontal position based on alignment
            float x = padding_;
            float child_width = width() - padding_ * 2;
            
            switch (alignment_) {
                case LayoutAlign::start:
                    x = padding_;
                    child_width = child->preferred_size().w;
                    break;
                    
                case LayoutAlign::center:
                    child_width = child->preferred_size().w;
                    x = (width() - child_width) / 2.0f;
                    break;
                    
                case LayoutAlign::end:
                    child_width = child->preferred_size().w;
                    x = width() - padding_ - child_width;
                    break;
                    
                case LayoutAlign::stretch:
                    x = padding_;
                    child_width = width() - padding_ * 2;
                    break;
            }
            
            // Set child bounds
            child->set_bounds(Rectf{x, y, child_width, child_height});
            
            // Layout child's children
            child->layout();
            
            y += child_height + spacing_;
        }
    }
};

/**
 * @brief Helpers to create layout containers
 */
inline WidgetPtr make_hbox() {
    return std::make_shared<HBox>();
}

inline WidgetPtr make_vbox() {
    return std::make_shared<VBox>();
}

inline WidgetPtr make_hbox(float spacing, float padding = 0.0f) {
    auto box = std::make_shared<HBox>();
    box->set_spacing(spacing);
    box->set_padding(padding);
    return box;
}

inline WidgetPtr make_vbox(float spacing, float padding = 0.0f) {
    auto box = std::make_shared<VBox>();
    box->set_spacing(spacing);
    box->set_padding(padding);
    return box;
}

} // namespace zuu::widget