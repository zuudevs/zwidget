#pragma once

/**
 * @file widget.hpp
 * @brief Base widget class - foundation for all UI components
 * @version 1.0
 * @date 2025-11-29
 */

#include "zwidget/unit/rect.hpp"
#include "zwidget/unit/color.hpp"
#include "zwidget/unit/event.hpp"
#include "zwidget/unit/align.hpp"
#include <memory>
#include <vector>

namespace zuu::widget {

// Forward declarations
class Widget;
class Canvas;
class RenderContext;

using WidgetPtr = std::shared_ptr<Widget>;
using WidgetList = std::vector<WidgetPtr>;

/**
 * @brief Widget state flags
 */
enum class WidgetState : uint32_t {
    none       = 0,
    visible    = 1 << 0,
    enabled    = 1 << 1,
    focused    = 1 << 2,
    hovered    = 1 << 3,
    pressed    = 1 << 4,
    dirty      = 1 << 5  // Needs repaint
};

inline constexpr WidgetState operator|(WidgetState a, WidgetState b) {
    return static_cast<WidgetState>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

inline constexpr WidgetState operator&(WidgetState a, WidgetState b) {
    return static_cast<WidgetState>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
    );
}

inline constexpr bool has_state(WidgetState flags, WidgetState check) {
    return (flags & check) == check;
}

/**
 * @brief Base widget class - all UI components inherit from this
 */
class Widget : public std::enable_shared_from_this<Widget> {
protected:
    Widget* parent_ = nullptr;
    WidgetList children_;
    
    Rectf bounds_;           // Position and size
    Color background_;
    Color foreground_;
    
    WidgetState state_ = WidgetState::visible | WidgetState::enabled;
    
    Align alignment_;
    
    // Layout hints
    Sizef min_size_{0, 0};
    Sizef max_size_{FLT_MAX, FLT_MAX};
    Sizef preferred_size_{100, 100};
    
public:
    virtual ~Widget() = default;
    
    // === Hierarchy Management ===
    
    /**
     * @brief Add child widget
     */
    virtual void add_child(WidgetPtr child) {
        if (child && child.get() != this) {
            child->parent_ = this;
            children_.push_back(std::move(child));
            mark_dirty();
        }
    }
    
    /**
     * @brief Remove child widget
     */
    virtual void remove_child(Widget* child) {
        auto it = std::find_if(children_.begin(), children_.end(),
            [child](const WidgetPtr& ptr) { return ptr.get() == child; });
        
        if (it != children_.end()) {
            (*it)->parent_ = nullptr;
            children_.erase(it);
            mark_dirty();
        }
    }
    
    /**
     * @brief Get parent widget
     */
    Widget* parent() const { return parent_; }
    
    /**
     * @brief Get children
     */
    const WidgetList& children() const { return children_; }
    
    // === Geometry ===
    
    void set_bounds(const Rectf& bounds) {
        if (bounds_.pos != bounds.pos || bounds_.size != bounds.size) {
            bounds_ = bounds;
            on_resize(bounds.size);
            mark_dirty();
        }
    }
    
    const Rectf& bounds() const { return bounds_; }
    
    void set_position(const Pointf& pos) {
        if (bounds_.pos != pos) {
            bounds_.pos = pos;
            mark_dirty();
        }
    }
    
    Pointf position() const { return bounds_.pos; }
    
    void set_size(const Sizef& size) {
        if (bounds_.size != size) {
            bounds_.size = size;
            on_resize(size);
            mark_dirty();
        }
    }
    
    Sizef size() const { return bounds_.size; }
    
    float width() const { return bounds_.width(); }
    float height() const { return bounds_.height(); }
    
    /**
     * @brief Get absolute position (relative to root)
     */
    Pointf absolute_position() const {
        Pointf pos = bounds_.pos;
        Widget* p = parent_;
        while (p) {
            pos += p->bounds_.pos;
            p = p->parent_;
        }
        return pos;
    }
    
    // === Layout ===
    
    void set_min_size(const Sizef& size) { min_size_ = size; mark_dirty(); }
    void set_max_size(const Sizef& size) { max_size_ = size; mark_dirty(); }
    void set_preferred_size(const Sizef& size) { preferred_size_ = size; mark_dirty(); }
    
    const Sizef& min_size() const { return min_size_; }
    const Sizef& max_size() const { return max_size_; }
    const Sizef& preferred_size() const { return preferred_size_; }
    
    void set_alignment(const Align& align) { alignment_ = align; mark_dirty(); }
    const Align& alignment() const { return alignment_; }
    
    /**
     * @brief Perform layout - override in containers
     */
    virtual void layout() {
        for (auto& child : children_) {
            if (child->is_visible()) {
                child->layout();
            }
        }
    }
    
    // === Appearance ===
    
    void set_background(const Color& color) {
        background_ = color;
        mark_dirty();
    }
    
    const Color& background() const { return background_; }
    
    void set_foreground(const Color& color) {
        foreground_ = color;
        mark_dirty();
    }
    
    const Color& foreground() const { return foreground_; }
    
    // === State Management ===
    
    void set_state(WidgetState state, bool value) {
        if (value) {
            state_ = state_ | state;
        } else {
            state_ = static_cast<WidgetState>(
                static_cast<uint32_t>(state_) & ~static_cast<uint32_t>(state)
            );
        }
        mark_dirty();
    }
    
    bool is_visible() const { return has_state(state_, WidgetState::visible); }
    bool is_enabled() const { return has_state(state_, WidgetState::enabled); }
    bool is_focused() const { return has_state(state_, WidgetState::focused); }
    bool is_hovered() const { return has_state(state_, WidgetState::hovered); }
    bool is_pressed() const { return has_state(state_, WidgetState::pressed); }
    bool is_dirty() const { return has_state(state_, WidgetState::dirty); }
    
    void set_visible(bool visible) { set_state(WidgetState::visible, visible); }
    void set_enabled(bool enabled) { set_state(WidgetState::enabled, enabled); }
    void set_focused(bool focused) { set_state(WidgetState::focused, focused); }
    
    void mark_dirty() {
        state_ = state_ | WidgetState::dirty;
        if (parent_) {
            parent_->mark_dirty();
        }
    }
    
    void clear_dirty() {
        state_ = static_cast<WidgetState>(
            static_cast<uint32_t>(state_) & ~static_cast<uint32_t>(WidgetState::dirty)
        );
    }
    
    // === Hit Testing ===
    
    /**
     * @brief Check if point is inside widget
     */
    bool contains(const Pointf& point) const {
        return bounds_.contains(point);
    }
    
    /**
     * @brief Find widget at point (recursive)
     */
    virtual Widget* hit_test(const Pointf& point) {
        if (!is_visible() || !contains(point)) {
            return nullptr;
        }
        
        // Check children first (reverse order - front to back)
        for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
            if (auto* hit = (*it)->hit_test(point - bounds_.pos)) {
                return hit;
            }
        }
        
        return this;
    }
    
    // === Event Handling ===
    
    /**
     * @brief Handle event - override in derived classes
     */
    virtual bool on_event(const Event& event) {
        // Dispatch to children first
        for (auto& child : children_) {
            if (child->on_event(event)) {
                return true;  // Event consumed
            }
        }
        
        // Handle based on event type
        if (event.is_mouse()) {
            if (auto* mouse = event.get_if<MouseEvent>()) {
                return on_mouse_event(*mouse);
            }
        } else if (event.is_keyboard()) {
            if (auto* keyboard = event.get_if<KeyboardEvent>()) {
                return on_keyboard_event(*keyboard);
            }
        }
        
        return false;  // Event not consumed
    }
    
    virtual bool on_mouse_event(const MouseEvent& event) {
        Pointf local_pos = event.position - absolute_position();
        
        switch (event.state) {
            case mouse_state::move:
                return on_mouse_move(local_pos);
            case mouse_state::press:
                return on_mouse_press(event.button, local_pos);
            case mouse_state::release:
                return on_mouse_release(event.button, local_pos);
            case mouse_state::enter:
                return on_mouse_enter();
            case mouse_state::leave:
                return on_mouse_leave();
            default:
                return false;
        }
    }
    
    virtual bool on_keyboard_event(const KeyboardEvent& event) {
        if (event.state == keyboard_state::press) {
            return on_key_press(event.key_code);
        } else if (event.state == keyboard_state::release) {
            return on_key_release(event.key_code);
        }
        return false;
    }
    
    // Event handlers - override these in derived classes
    virtual bool on_mouse_move(const Pointf& pos) { return false; }
    virtual bool on_mouse_press(mouse_button button, const Pointf& pos) { return false; }
    virtual bool on_mouse_release(mouse_button button, const Pointf& pos) { return false; }
    virtual bool on_mouse_enter() { 
        set_state(WidgetState::hovered, true);
        return false; 
    }
    virtual bool on_mouse_leave() { 
        set_state(WidgetState::hovered, false);
        return false; 
    }
    virtual bool on_key_press(uint32_t key) { return false; }
    virtual bool on_key_release(uint32_t key) { return false; }
    virtual void on_resize(const Sizef& new_size) {}
    
    // === Rendering ===
    
    /**
     * @brief Render widget and children
     */
    virtual void render(Canvas& canvas) {
        if (!is_visible()) return;
        
        // Draw self
        draw(canvas);
        
        // Draw children
        for (auto& child : children_) {
            child->render(canvas);
        }
        
        clear_dirty();
    }
    
    /**
     * @brief Draw widget content - override in derived classes
     */
    virtual void draw(Canvas& canvas) {
        // Default: fill background
        // Canvas implementation akan di-implement nanti
    }
};

/**
 * @brief Helper to create widgets
 */
template <typename T, typename... Args>
inline WidgetPtr make_widget(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

} // namespace zuu::widget
