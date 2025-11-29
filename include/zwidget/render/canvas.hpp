#pragma once

/**
 * @file canvas.hpp
 * @brief High-level canvas wrapper for widget rendering
 * @version 1.0
 * @date 2025-11-29
 */

#include "context.hpp"

namespace zuu::widget {

/**
 * @brief High-level canvas for widget rendering
 * Provides convenient drawing methods with automatic coordinate translation
 */
class Canvas {
private:
    RenderContext* context_;
    Pointf origin_;  // Current drawing origin
    
public:
    explicit Canvas(RenderContext& ctx) : context_(&ctx), origin_{0, 0} {}
    
    /**
     * @brief Get underlying render context
     */
    RenderContext& context() { return *context_; }
    const RenderContext& context() const { return *context_; }
    
    /**
     * @brief Get current origin
     */
    Pointf origin() const { return origin_; }
    
    /**
     * @brief Set drawing origin (for nested widgets)
     */
    void set_origin(const Pointf& pos) { origin_ = pos; }
    
    /**
     * @brief Translate origin
     */
    void translate(const Pointf& offset) { origin_ += offset; }
    
    /**
     * @brief Reset origin to (0, 0)
     */
    void reset_origin() { origin_ = {0, 0}; }
    
    // === Convenience Drawing Methods ===
    // All coordinates are relative to current origin
    
    /**
     * @brief Clear with color
     */
    void clear(const Color& color) {
        context_->clear(color);
    }
    
    /**
     * @brief Draw line
     */
    void draw_line(
        const Pointf& start,
        const Pointf& end,
        const Color& color,
        float width = 1.0f
    ) {
        context_->draw_line(start + origin_, end + origin_, color, width);
    }
    
    /**
     * @brief Draw rectangle
     */
    void draw_rect(const Rectf& rect, const Color& color, float width = 1.0f) {
        Rectf r = rect;
        r.pos += origin_;
        context_->draw_rect(r, color, width);
    }
    
    /**
     * @brief Fill rectangle
     */
    void fill_rect(const Rectf& rect, const Color& color) {
        Rectf r = rect;
        r.pos += origin_;
        context_->fill_rect(r, color);
    }
    
    /**
     * @brief Draw rounded rectangle
     */
    void draw_rounded_rect(
        const Rectf& rect,
        float radius,
        const Color& color,
        float width = 1.0f
    ) {
        Rectf r = rect;
        r.pos += origin_;
        context_->draw_rounded_rect(r, radius, radius, color, width);
    }
    
    /**
     * @brief Fill rounded rectangle
     */
    void fill_rounded_rect(
        const Rectf& rect,
        float radius,
        const Color& color
    ) {
        Rectf r = rect;
        r.pos += origin_;
        context_->fill_rounded_rect(r, radius, radius, color);
    }
    
    /**
     * @brief Draw circle
     */
    void draw_circle(
        const Pointf& center,
        float radius,
        const Color& color,
        float width = 1.0f
    ) {
        context_->draw_circle(center + origin_, radius, color, width);
    }
    
    /**
     * @brief Fill circle
     */
    void fill_circle(
        const Pointf& center,
        float radius,
        const Color& color
    ) {
        context_->fill_circle(center + origin_, radius, color);
    }
    
    /**
     * @brief Draw text at position
     */
    void draw_text(
        const std::wstring& text,
        const Pointf& position,
        const Color& color,
        const TextStyle& style = TextStyle()
    ) {
        context_->draw_text(text, position + origin_, color, style);
    }
    
    /**
     * @brief Draw text in rectangle
     */
    void draw_text(
        const std::wstring& text,
        const Rectf& rect,
        const Color& color,
        const TextStyle& style = TextStyle()
    ) {
        Rectf r = rect;
        r.pos += origin_;
        context_->draw_text(text, r, color, style);
    }
    
    /**
     * @brief Measure text
     */
    Sizef measure_text(
        const std::wstring& text,
        const TextStyle& style = TextStyle()
    ) {
        return context_->measure_text(text, style);
    }
    
    /**
     * @brief Draw image
     */
    void draw_image(
        const Image& image,
        const Pointf& position,
        float opacity = 1.0f
    ) {
        context_->draw_image(image, position + origin_, opacity);
    }
    
    /**
     * @brief Draw image in rectangle
     */
    void draw_image(
        const Image& image,
        const Rectf& rect,
        float opacity = 1.0f
    ) {
        Rectf r = rect;
        r.pos += origin_;
        context_->draw_image(image, r, opacity);
    }
    
    /**
     * @brief Fill with gradient
     */
    void fill_rect_gradient(
        const Rectf& rect,
        const Color& start_color,
        const Color& end_color,
        const Pointf& start_point,
        const Pointf& end_point
    ) {
        Rectf r = rect;
        r.pos += origin_;
        context_->fill_rect_gradient(
            r, start_color, end_color,
            start_point + origin_, end_point + origin_
        );
    }
    
    // === Advanced Methods ===
    
    /**
     * @brief Set clipping rectangle
     */
    void set_clip(const Rectf& rect) {
        Rectf r = rect;
        r.pos += origin_;
        context_->set_clip_rect(r);
    }
    
    /**
     * @brief Reset clipping
     */
    void reset_clip() {
        context_->reset_clip();
    }
    
    /**
     * @brief Save state (includes origin)
     */
    void save() {
        context_->save_state();
    }
    
    /**
     * @brief Restore state (includes origin)
     */
    void restore() {
        context_->restore_state();
    }
};

/**
 * @brief RAII helper for canvas translation
 */
class CanvasTranslate {
private:
    Canvas* canvas_;
    Pointf old_origin_;
    
public:
    CanvasTranslate(Canvas& canvas, const Pointf& offset)
        : canvas_(&canvas)
        , old_origin_(canvas.origin())
    {
        canvas_->translate(offset);
    }
    
    ~CanvasTranslate() {
        if (canvas_) {
            canvas_->set_origin(old_origin_);
        }
    }
    
    CanvasTranslate(const CanvasTranslate&) = delete;
    CanvasTranslate& operator=(const CanvasTranslate&) = delete;
    
    CanvasTranslate(CanvasTranslate&& other) noexcept
        : canvas_(other.canvas_)
        , old_origin_(other.old_origin_)
    {
        other.canvas_ = nullptr;
    }
};

/**
 * @brief RAII helper for canvas clipping
 */
class CanvasClip {
private:
    Canvas* canvas_;
    
public:
    CanvasClip(Canvas& canvas, const Rectf& rect) : canvas_(&canvas) {
        canvas_->set_clip(rect);
    }
    
    ~CanvasClip() {
        if (canvas_) {
            canvas_->reset_clip();
        }
    }
    
    CanvasClip(const CanvasClip&) = delete;
    CanvasClip& operator=(const CanvasClip&) = delete;
    
    CanvasClip(CanvasClip&& other) noexcept : canvas_(other.canvas_) {
        other.canvas_ = nullptr;
    }
};

} // namespace zuu::widget
