#pragma once

/**
 * @file label.hpp
 * @brief Simple text label widget
 * @version 1.0
 * @date 2025-11-30
 */

#include "zwidget/core/widget.hpp"
#include "zwidget/render/canvas.hpp"

namespace zuu::widget {

/**
 * @brief Text label widget - displays static or dynamic text
 */
class Label : public Widget {
private:
    std::wstring text_;
    TextStyle text_style_;
    bool word_wrap_ = false;
    
public:
    Label() {
        set_preferred_size(Sizef{100, 30});
        set_background(Color::transparent());
        set_foreground(Color::black());
        
        text_style_.font_size = 12.0f;
        text_style_.align = TextAlign::left;
        text_style_.valign = TextVAlign::middle;
    }
    
    explicit Label(const std::wstring& text) : Label() {
        text_ = text;
    }
    
    Label(const std::wstring& text, const TextStyle& style) : Label(text) {
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
    
    void set_italic(bool italic) {
        text_style_.italic = italic;
        mark_dirty();
    }
    
    void set_alignment(TextAlign align) {
        text_style_.align = align;
        mark_dirty();
    }
    
    void set_vertical_alignment(TextVAlign valign) {
        text_style_.valign = valign;
        mark_dirty();
    }
    
    void set_word_wrap(bool wrap) {
        word_wrap_ = wrap;
        mark_dirty();
    }
    
    bool word_wrap() const { return word_wrap_; }
    
    // === Widget Interface ===
    
    void draw(Canvas& canvas) override {
        if (text_.empty()) return;
        
        // Draw background if not transparent
        if (background().a > 0) {
            canvas.fill_rect(Rectf{0, 0, width(), height()}, background());
        }
        
        // Draw text
        Rectf text_rect{0, 0, width(), height()};
        canvas.draw_text(text_, text_rect, foreground(), text_style_);
    }
    
    // Calculate preferred size based on text
    void auto_size() {
        // This would need RenderContext access to measure text
        // For now, just mark dirty
        mark_dirty();
    }
};

/**
 * @brief Helper to create label
 */
inline WidgetPtr make_label(const std::wstring& text) {
    return std::make_shared<Label>(text);
}

inline WidgetPtr make_label(const std::wstring& text, const TextStyle& style) {
    return std::make_shared<Label>(text, style);
}

} // namespace zuu::widget
