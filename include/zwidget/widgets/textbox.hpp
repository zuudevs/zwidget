#pragma once

/**
 * @file textbox.hpp
 * @brief Single-line text input widget
 * @version 1.0
 * @date 2025-11-30
 */

#include "zwidget/core/widget.hpp"
#include "zwidget/render/canvas.hpp"
#include <functional>

namespace zuu::widget {

/**
 * @brief Single-line text input box
 */
class TextBox : public Widget {
public:
    using TextChangedCallback = std::function<void(const std::wstring&)>;
    using TextSubmitCallback = std::function<void(const std::wstring&)>;
    
private:
    std::wstring text_;
    std::wstring placeholder_;
    size_t cursor_pos_ = 0;
    size_t selection_start_ = 0;
    size_t selection_end_ = 0;
    
    TextStyle text_style_;
    Color placeholder_color_{128, 128, 128, 255};
    Color selection_color_{100, 150, 255, 100};
    Color cursor_color_{0, 0, 0, 255};
    
    bool show_cursor_ = true;
    float cursor_blink_time_ = 0.0f;
    float cursor_blink_interval_ = 0.5f;
    
    bool read_only_ = false;
    size_t max_length_ = 1000;
    
    TextChangedCallback on_text_changed_;
    TextSubmitCallback on_submit_;
    
public:
    TextBox() {
        set_preferred_size(Sizef{200, 30});
        set_background(Color::white());
        set_foreground(Color::black());
        
        text_style_.font_size = 12.0f;
        text_style_.align = TextAlign::left;
        text_style_.valign = TextVAlign::middle;
    }
    
    explicit TextBox(const std::wstring& initial_text) : TextBox() {
        text_ = initial_text;
        cursor_pos_ = text_.length();
    }
    
    // === Text Management ===
    
    void set_text(const std::wstring& text) {
        if (text_ != text) {
            text_ = text;
            cursor_pos_ = std::min(cursor_pos_, text_.length());
            selection_start_ = selection_end_ = cursor_pos_;
            mark_dirty();
            
            if (on_text_changed_) {
                on_text_changed_(text_);
            }
        }
    }
    
    const std::wstring& text() const { return text_; }
    
    void set_placeholder(const std::wstring& placeholder) {
        placeholder_ = placeholder;
        mark_dirty();
    }
    
    const std::wstring& placeholder() const { return placeholder_; }
    
    void set_read_only(bool read_only) {
        read_only_ = read_only;
    }
    
    bool is_read_only() const { return read_only_; }
    
    void set_max_length(size_t length) {
        max_length_ = length;
    }
    
    size_t max_length() const { return max_length_; }
    
    // === Callbacks ===
    
    void set_on_text_changed(TextChangedCallback callback) {
        on_text_changed_ = std::move(callback);
    }
    
    void set_on_submit(TextSubmitCallback callback) {
        on_submit_ = std::move(callback);
    }
    
    // === Selection ===
    
    void select_all() {
        selection_start_ = 0;
        selection_end_ = text_.length();
        cursor_pos_ = selection_end_;
        mark_dirty();
    }
    
    void clear_selection() {
        selection_start_ = selection_end_ = cursor_pos_;
        mark_dirty();
    }
    
    bool has_selection() const {
        return selection_start_ != selection_end_;
    }
    
    std::wstring get_selected_text() const {
        if (!has_selection()) return L"";
        size_t start = std::min(selection_start_, selection_end_);
        size_t end = std::max(selection_start_, selection_end_);
        return text_.substr(start, end - start);
    }
    
    // === Widget Interface ===
    
    void draw(Canvas& canvas) override {
        Rectf bounds{0, 0, width(), height()};
        
        // Draw background
        canvas.fill_rect(bounds, background());
        
        // Draw border
        Color border_color = is_focused() ? 
            Color(0, 120, 215, 255) : Color(200, 200, 200, 255);
        canvas.draw_rect(bounds, border_color, is_focused() ? 2.0f : 1.0f);
        
        // Prepare text rect with padding
        Rectf text_rect{5, 0, width() - 10, height()};
        
        // Draw selection
        if (has_selection() && is_focused()) {
            // Simplified: draw selection background
            // In real implementation, would calculate exact text positions
            canvas.fill_rect(
                Rectf{text_rect.pos.x, text_rect.pos.y, 
                      text_rect.size.w * 0.3f, text_rect.size.h},
                selection_color_
            );
        }
        
        // Draw text or placeholder
        if (text_.empty() && !placeholder_.empty() && !is_focused()) {
            canvas.draw_text(placeholder_, text_rect, placeholder_color_, text_style_);
        } else {
            canvas.draw_text(text_, text_rect, foreground(), text_style_);
        }
        
        // Draw cursor
        if (is_focused() && show_cursor_ && !read_only_) {
            float cursor_x = text_rect.pos.x + 5; // Simplified position
            canvas.draw_line(
                Pointf{cursor_x, text_rect.pos.y + 5},
                Pointf{cursor_x, text_rect.pos.y + height() - 10},
                cursor_color_,
                2.0f
            );
        }
    }
    
    bool on_mouse_press(mouse_button button, const Pointf& pos) override {
        if (button == mouse_button::left) {
            set_focused(true);
            
            // Calculate cursor position from mouse click
            // Simplified: just set to end
            cursor_pos_ = text_.length();
            clear_selection();
            
            mark_dirty();
            return true;
        }
        return false;
    }
    
    bool on_key_press(uint32_t key) override {
        if (read_only_) return false;
        
        bool ctrl = GetKeyState(VK_CONTROL) & 0x8000;
        bool shift = GetKeyState(VK_SHIFT) & 0x8000;
        
        switch (key) {
            case VK_LEFT:
                move_cursor_left(shift);
                return true;
                
            case VK_RIGHT:
                move_cursor_right(shift);
                return true;
                
            case VK_HOME:
                move_cursor_home(shift);
                return true;
                
            case VK_END:
                move_cursor_end(shift);
                return true;
                
            case VK_BACK:
                if (has_selection()) {
                    delete_selection();
                } else if (cursor_pos_ > 0) {
                    text_.erase(cursor_pos_ - 1, 1);
                    cursor_pos_--;
                    mark_dirty();
                    if (on_text_changed_) on_text_changed_(text_);
                }
                return true;
                
            case VK_DELETE:
                if (has_selection()) {
                    delete_selection();
                } else if (cursor_pos_ < text_.length()) {
                    text_.erase(cursor_pos_, 1);
                    mark_dirty();
                    if (on_text_changed_) on_text_changed_(text_);
                }
                return true;
                
            case VK_RETURN:
                if (on_submit_) {
                    on_submit_(text_);
                }
                return true;
                
            case 'A':
                if (ctrl) {
                    select_all();
                    return true;
                }
                break;
                
            case 'C':
                if (ctrl && has_selection()) {
                    // Copy to clipboard (simplified)
                    return true;
                }
                break;
                
            case 'V':
                if (ctrl) {
                    // Paste from clipboard (simplified)
                    return true;
                }
                break;
                
            case 'X':
                if (ctrl && has_selection()) {
                    // Cut to clipboard (simplified)
                    delete_selection();
                    return true;
                }
                break;
        }
        
        // Handle character input
        if (key >= 32 && key <= 126) {
            wchar_t ch = static_cast<wchar_t>(key);
            if (shift) {
                // Handle shift modifier for characters
                // Simplified: just use uppercase
                if (ch >= 'a' && ch <= 'z') {
                    ch = ch - 'a' + 'A';
                }
            } else if (ch >= 'A' && ch <= 'Z') {
                ch = ch - 'A' + 'a';
            }
            
            insert_text(std::wstring(1, ch));
            return true;
        }
        
        return false;
    }
    
private:
    void move_cursor_left(bool select = false) {
        if (cursor_pos_ > 0) {
            cursor_pos_--;
            if (!select) {
                clear_selection();
            } else {
                selection_end_ = cursor_pos_;
            }
            mark_dirty();
        }
    }
    
    void move_cursor_right(bool select = false) {
        if (cursor_pos_ < text_.length()) {
            cursor_pos_++;
            if (!select) {
                clear_selection();
            } else {
                selection_end_ = cursor_pos_;
            }
            mark_dirty();
        }
    }
    
    void move_cursor_home(bool select = false) {
        cursor_pos_ = 0;
        if (!select) {
            clear_selection();
        } else {
            selection_end_ = cursor_pos_;
        }
        mark_dirty();
    }
    
    void move_cursor_end(bool select = false) {
        cursor_pos_ = text_.length();
        if (!select) {
            clear_selection();
        } else {
            selection_end_ = cursor_pos_;
        }
        mark_dirty();
    }
    
    void delete_selection() {
        if (!has_selection()) return;
        
        size_t start = std::min(selection_start_, selection_end_);
        size_t end = std::max(selection_start_, selection_end_);
        
        text_.erase(start, end - start);
        cursor_pos_ = start;
        clear_selection();
        mark_dirty();
        
        if (on_text_changed_) {
            on_text_changed_(text_);
        }
    }
    
    void insert_text(const std::wstring& text) {
        if (text_.length() + text.length() > max_length_) return;
        
        if (has_selection()) {
            delete_selection();
        }
        
        text_.insert(cursor_pos_, text);
        cursor_pos_ += text.length();
        mark_dirty();
        
        if (on_text_changed_) {
            on_text_changed_(text_);
        }
    }
};

/**
 * @brief Helper to create textbox
 */
inline WidgetPtr make_textbox(const std::wstring& initial_text = L"") {
    return std::make_shared<TextBox>(initial_text);
}

} // namespace zuu::widget
