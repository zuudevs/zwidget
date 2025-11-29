#pragma once

/**
 * @file context.hpp
 * @brief Abstract rendering context interface
 * @version 1.0
 * @date 2025-11-29
 */

#include "zwidget/unit/rect.hpp"
#include "zwidget/unit/color.hpp"
#include "zwidget/unit/point.hpp"
#include <string>
#include <vector>

namespace zuu::widget {

// Forward declarations
class RenderTarget;
class Brush;
class Font;
class Image;

/**
 * @brief Text alignment options
 */
enum class TextAlign : uint8_t {
    left,
    center,
    right
};

enum class TextVAlign : uint8_t {
    top,
    middle,
    bottom
};

/**
 * @brief Line cap styles
 */
enum class LineCap : uint8_t {
    flat,
    square,
    round,
    triangle
};

/**
 * @brief Line join styles
 */
enum class LineJoin : uint8_t {
    miter,
    bevel,
    round,
    miter_or_bevel
};

/**
 * @brief Drawing style for strokes
 */
struct StrokeStyle {
    float width = 1.0f;
    LineCap start_cap = LineCap::flat;
    LineCap end_cap = LineCap::flat;
    LineCap dash_cap = LineCap::flat;
    LineJoin line_join = LineJoin::miter;
    float miter_limit = 10.0f;
    std::vector<float> dashes;
    float dash_offset = 0.0f;
    
    StrokeStyle() = default;
    explicit StrokeStyle(float w) : width(w) {}
};

/**
 * @brief Text rendering options
 */
struct TextStyle {
    std::wstring font_family = L"Segoe UI";
    float font_size = 12.0f;
    bool bold = false;
    bool italic = false;
    bool underline = false;
    bool strikethrough = false;
    TextAlign align = TextAlign::left;
    TextVAlign valign = TextVAlign::top;
    
    TextStyle() = default;
    TextStyle(std::wstring family, float size) 
        : font_family(std::move(family)), font_size(size) {}
};

/**
 * @brief Abstract rendering context
 * Platform-independent drawing interface
 */
class RenderContext {
public:
    virtual ~RenderContext() = default;
    
    // === State Management ===
    
    /**
     * @brief Begin drawing - must be called before any draw operations
     */
    virtual void begin_draw() = 0;
    
    /**
     * @brief End drawing and present
     */
    virtual void end_draw() = 0;
    
    /**
     * @brief Clear entire surface with color
     */
    virtual void clear(const Color& color) = 0;
    
    /**
     * @brief Save current transform and clip state
     */
    virtual void save_state() = 0;
    
    /**
     * @brief Restore previously saved state
     */
    virtual void restore_state() = 0;
    
    // === Transform ===
    
    /**
     * @brief Translate coordinate system
     */
    virtual void translate(float x, float y) = 0;
    
    /**
     * @brief Scale coordinate system
     */
    virtual void scale(float sx, float sy) = 0;
    
    /**
     * @brief Rotate coordinate system (radians)
     */
    virtual void rotate(float radians) = 0;
    
    /**
     * @brief Reset transform to identity
     */
    virtual void reset_transform() = 0;
    
    // === Clipping ===
    
    /**
     * @brief Set clipping rectangle
     */
    virtual void set_clip_rect(const Rectf& rect) = 0;
    
    /**
     * @brief Reset clipping region
     */
    virtual void reset_clip() = 0;
    
    // === Basic Shapes ===
    
    /**
     * @brief Draw line
     */
    virtual void draw_line(
        const Pointf& start,
        const Pointf& end,
        const Color& color,
        float width = 1.0f
    ) = 0;
    
    /**
     * @brief Draw rectangle outline
     */
    virtual void draw_rect(
        const Rectf& rect,
        const Color& color,
        float width = 1.0f
    ) = 0;
    
    /**
     * @brief Fill rectangle
     */
    virtual void fill_rect(
        const Rectf& rect,
        const Color& color
    ) = 0;
    
    /**
     * @brief Draw rounded rectangle outline
     */
    virtual void draw_rounded_rect(
        const Rectf& rect,
        float radius_x,
        float radius_y,
        const Color& color,
        float width = 1.0f
    ) = 0;
    
    /**
     * @brief Fill rounded rectangle
     */
    virtual void fill_rounded_rect(
        const Rectf& rect,
        float radius_x,
        float radius_y,
        const Color& color
    ) = 0;
    
    /**
     * @brief Draw ellipse outline
     */
    virtual void draw_ellipse(
        const Pointf& center,
        float radius_x,
        float radius_y,
        const Color& color,
        float width = 1.0f
    ) = 0;
    
    /**
     * @brief Fill ellipse
     */
    virtual void fill_ellipse(
        const Pointf& center,
        float radius_x,
        float radius_y,
        const Color& color
    ) = 0;
    
    /**
     * @brief Draw circle outline
     */
    void draw_circle(
        const Pointf& center,
        float radius,
        const Color& color,
        float width = 1.0f
    ) {
        draw_ellipse(center, radius, radius, color, width);
    }
    
    /**
     * @brief Fill circle
     */
    void fill_circle(
        const Pointf& center,
        float radius,
        const Color& color
    ) {
        fill_ellipse(center, radius, radius, color);
    }
    
    // === Advanced Shapes ===
    
    /**
     * @brief Draw polyline
     */
    virtual void draw_polyline(
        const std::vector<Pointf>& points,
        const Color& color,
        float width = 1.0f,
        bool closed = false
    ) = 0;
    
    /**
     * @brief Fill polygon
     */
    virtual void fill_polygon(
        const std::vector<Pointf>& points,
        const Color& color
    ) = 0;
    
    // === Text Rendering ===
    
    /**
     * @brief Draw text at position
     */
    virtual void draw_text(
        const std::wstring& text,
        const Pointf& position,
        const Color& color,
        const TextStyle& style = TextStyle()
    ) = 0;
    
    /**
     * @brief Draw text in rectangle with alignment
     */
    virtual void draw_text(
        const std::wstring& text,
        const Rectf& rect,
        const Color& color,
        const TextStyle& style = TextStyle()
    ) = 0;
    
    /**
     * @brief Measure text dimensions
     */
    virtual Sizef measure_text(
        const std::wstring& text,
        const TextStyle& style = TextStyle()
    ) = 0;
    
    // === Image Rendering ===
    
    /**
     * @brief Draw image at position
     */
    virtual void draw_image(
        const Image& image,
        const Pointf& position,
        float opacity = 1.0f
    ) = 0;
    
    /**
     * @brief Draw image in rectangle (stretched)
     */
    virtual void draw_image(
        const Image& image,
        const Rectf& dest_rect,
        float opacity = 1.0f
    ) = 0;
    
    /**
     * @brief Draw part of image
     */
    virtual void draw_image(
        const Image& image,
        const Rectf& dest_rect,
        const Rectf& source_rect,
        float opacity = 1.0f
    ) = 0;
    
    // === Gradients ===
    
    /**
     * @brief Fill rectangle with linear gradient
     */
    virtual void fill_rect_gradient(
        const Rectf& rect,
        const Color& start_color,
        const Color& end_color,
        const Pointf& start_point,
        const Pointf& end_point
    ) = 0;
    
    /**
     * @brief Fill rectangle with radial gradient
     */
    virtual void fill_rect_radial_gradient(
        const Rectf& rect,
        const Color& center_color,
        const Color& edge_color,
        const Pointf& center,
        float radius_x,
        float radius_y
    ) = 0;
    
    // === Properties ===
    
    /**
     * @brief Get render target size
     */
    virtual Sizef get_size() const = 0;
    
    /**
     * @brief Get DPI scale factor
     */
    virtual float get_dpi_scale() const = 0;
    
    /**
     * @brief Check if rendering is active
     */
    virtual bool is_drawing() const = 0;
    
    // === Resource Management ===
    
    /**
     * @brief Resize render target
     */
    virtual void resize(const Size& new_size) = 0;
    
    /**
     * @brief Flush pending operations
     */
    virtual void flush() = 0;
};

/**
 * @brief RAII helper for begin/end draw
 */
class DrawScope {
private:
    RenderContext* ctx_;
    
public:
    explicit DrawScope(RenderContext& ctx) : ctx_(&ctx) {
        ctx_->begin_draw();
    }
    
    ~DrawScope() {
        if (ctx_) {
            ctx_->end_draw();
        }
    }
    
    DrawScope(const DrawScope&) = delete;
    DrawScope& operator=(const DrawScope&) = delete;
    
    DrawScope(DrawScope&& other) noexcept : ctx_(other.ctx_) {
        other.ctx_ = nullptr;
    }
    
    DrawScope& operator=(DrawScope&& other) noexcept {
        if (this != &other) {
            if (ctx_) ctx_->end_draw();
            ctx_ = other.ctx_;
            other.ctx_ = nullptr;
        }
        return *this;
    }
};

/**
 * @brief RAII helper for save/restore state
 */
class StateScope {
private:
    RenderContext* ctx_;
    
public:
    explicit StateScope(RenderContext& ctx) : ctx_(&ctx) {
        ctx_->save_state();
    }
    
    ~StateScope() {
        if (ctx_) {
            ctx_->restore_state();
        }
    }
    
    StateScope(const StateScope&) = delete;
    StateScope& operator=(const StateScope&) = delete;
    
    StateScope(StateScope&& other) noexcept : ctx_(other.ctx_) {
        other.ctx_ = nullptr;
    }
    
    StateScope& operator=(StateScope&& other) noexcept {
        if (this != &other) {
            if (ctx_) ctx_->restore_state();
            ctx_ = other.ctx_;
            other.ctx_ = nullptr;
        }
        return *this;
    }
};

} // namespace zuu::widget
