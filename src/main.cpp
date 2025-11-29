/**
 * @file render_demo.cpp
 * @brief Complete rendering demonstration with Direct2D
 */

#include "zwidget/core/window.hpp"
#include "zwidget/core/widget.hpp"
#include "zwidget/render/d2d/context.hpp"
#include "zwidget/render/canvas.hpp"
#include <iostream>
#include <cmath>
#include <chrono>

using namespace zuu::widget;

/**
 * @brief Simple button widget with rendering
 */
class Button : public Widget {
private:
    std::wstring text_;
    bool is_hovered_ = false;
    bool is_pressed_ = false;
    
public:
    Button(std::wstring text) : text_(std::move(text)) {
        set_preferred_size(Sizef{120, 40});
        set_background(Color::white());
        set_foreground(Color::black());
    }
    
    void draw(Canvas& canvas) override {
        auto rect = Rectf{0, 0, width(), height()};
        
        // Determine colors based on state
        Color bg_color = background();
        Color border_color = foreground();
        
        if (is_pressed_) {
            bg_color = Color(200, 200, 200, 255);
        } else if (is_hovered_) {
            bg_color = Color(230, 230, 230, 255);
        }
        
        // Draw background
        canvas.fill_rounded_rect(rect, 4.0f, bg_color);
        
        // Draw border
        canvas.draw_rounded_rect(rect, 4.0f, border_color, 1.0f);
        
        // Draw text
        TextStyle style;
        style.font_size = 14.0f;
        style.align = TextAlign::center;
        style.valign = TextVAlign::middle;
        
        canvas.draw_text(text_, rect, foreground(), style);
    }
    
    bool on_mouse_enter() override {
        is_hovered_ = true;
        mark_dirty();
        return false;
    }
    
    bool on_mouse_leave() override {
        is_hovered_ = false;
        is_pressed_ = false;
        mark_dirty();
        return false;
    }
    
    bool on_mouse_press(mouse_button button, const Pointf& pos) override {
        if (button == mouse_button::left) {
            is_pressed_ = true;
            mark_dirty();
            std::wcout << L"Button clicked: " << text_ << L"\n";
            return true;
        }
        return false;
    }
    
    bool on_mouse_release(mouse_button button, const Pointf& pos) override {
        if (button == mouse_button::left) {
            is_pressed_ = false;
            mark_dirty();
            return true;
        }
        return false;
    }
};

/**
 * @brief Demo scene with various shapes
 */
class DemoScene {
private:
    float animation_time_ = 0.0f;
    
public:
    void update(float dt) {
        animation_time_ += dt;
    }
    
    void draw_shapes(Canvas& canvas) {
        // Title
        TextStyle title_style;
        title_style.font_size = 24.0f;
        title_style.bold = true;
        canvas.draw_text(
            L"ZWidget Rendering Demo",
            Pointf{20, 20},
            Color::black(),
            title_style
        );
        
        // Rectangles
        canvas.fill_rect(Rectf{20, 80, 100, 60}, Color::red());
        canvas.draw_rect(Rectf{140, 80, 100, 60}, Color::green(), 2.0f);
        
        // Rounded rectangles
        canvas.fill_rounded_rect(Rectf{260, 80, 100, 60}, 10.0f, Color::blue());
        canvas.draw_rounded_rect(Rectf{380, 80, 100, 60}, 10.0f, Color::magenta(), 2.0f);
        
        // Circles
        canvas.fill_circle(Pointf{70, 220}, 30.0f, Color::cyan());
        canvas.draw_circle(Pointf{190, 220}, 30.0f, Color::yellow(), 2.0f);
        
        // Animated circle
        float radius = 20.0f + std::sin(animation_time_ * 2.0f) * 10.0f;
        canvas.fill_circle(Pointf{310, 220}, radius, Color(255, 150, 0, 255));
        
        // Lines
        for (int i = 0; i < 5; ++i) {
            float x = 20 + i * 30;
            canvas.draw_line(
                Pointf{x, 300},
                Pointf{x + 20, 350},
                Color(i * 50, 100, 255 - i * 50, 255),
                2.0f + i
            );
        }
        
        // Text with different styles
        TextStyle normal_style;
        normal_style.font_size = 14.0f;
        
        TextStyle bold_style;
        bold_style.font_size = 14.0f;
        bold_style.bold = true;
        
        TextStyle italic_style;
        italic_style.font_size = 14.0f;
        italic_style.italic = true;
        
        canvas.draw_text(L"Normal Text", Pointf{20, 380}, Color::black(), normal_style);
        canvas.draw_text(L"Bold Text", Pointf{150, 380}, Color::black(), bold_style);
        canvas.draw_text(L"Italic Text", Pointf{250, 380}, Color::black(), italic_style);
        
        // Gradient (placeholder visualization)
        for (int i = 0; i < 100; ++i) {
            uint8_t val = static_cast<uint8_t>(i * 255 / 100);
            canvas.fill_rect(
                Rectf{20.0f + i * 2.0f, 420.0f, 2, 30},
                Color(val, 0, 255 - val, 255)
            );
        }
        
        // Performance info
        TextStyle info_style;
        info_style.font_size = 12.0f;
        info_style.align = TextAlign::right;
        
        canvas.draw_text(
            L"Direct2D Rendering",
            Rectf{0, 10, 760, 30},
            Color(100, 100, 100, 255),
            info_style
        );
    }
};

int main() {
    try {
        std::wcout << L"=== ZWidget Rendering Demo ===\n\n";
        
        // Create window
        WindowConfig config;
        config.title = L"ZWidget Rendering Demo";
        config.size = Size{800, 600};
        
        Window window(config);
        
        // Create D2D render context
        D2DContext render_ctx(window.native_handle());
        Canvas canvas(render_ctx);
        
        // Create demo scene
        DemoScene scene;
        
        // Create widgets
        auto root = make_widget<Widget>();
        root->set_bounds(Rectf{0, 0, 800, 600});
        
        auto button1 = make_widget<Button>(L"Click Me!");
        button1->set_bounds(Rectf{500, 100, 120, 40});
        root->add_child(button1);
        
        auto button2 = make_widget<Button>(L"Button 2");
        button2->set_bounds(Rectf{500, 160, 120, 40});
        root->add_child(button2);
        
        auto button3 = make_widget<Button>(L"Exit");
        button3->set_bounds(Rectf{500, 220, 120, 40});
        root->add_child(button3);
        
        // Event handling
        Widget* hovered_widget = nullptr;
        
        window.set_event_callback([&](const Event& event) {
            event.visit([&](auto&& evt) {
                using T = std::decay_t<decltype(evt)>;
                
                if constexpr (std::is_same_v<T, WindowEvent>) {
                    if (evt.state == window_state::close) {
                        window.set_should_close(true);
                    } else if (evt.state == window_state::resize) {
                        render_ctx.resize(evt.size);
                        root->set_size(Sizef{
                            static_cast<float>(evt.size.w),
                            static_cast<float>(evt.size.h)
                        });
                    }
                }
                else if constexpr (std::is_same_v<T, MouseEvent>) {
                    Pointf mouse_pos = evt.position;
                    
                    // Hit testing
                    Widget* hit = root->hit_test(mouse_pos);
                    
                    // Handle hover
                    if (hit != hovered_widget) {
                        if (hovered_widget) {
                            hovered_widget->on_mouse_leave();
                        }
                        if (hit) {
                            hit->on_mouse_enter();
                        }
                        hovered_widget = hit;
                    }
                    
                    // Dispatch event to widget
                    if (hit) {
                        hit->on_mouse_event(evt);
                        
                        // Check if exit button clicked
                        if (hit == button3.get() && 
                            evt.state == mouse_state::press &&
                            evt.button == mouse_button::left) {
                            window.set_should_close(true);
                        }
                    }
                }
                else if constexpr (std::is_same_v<T, KeyboardEvent>) {
                    if (evt.state == keyboard_state::press) {
                        if (evt.key_code == VK_ESCAPE) {
                            window.set_should_close(true);
                        }
                    }
                }
            });
        });
        
        // Timing
        auto last_time = std::chrono::high_resolution_clock::now();
        float fps = 0.0f;
        int frame_count = 0;
        
        std::wcout << L"Window created. Press ESC or click Exit to quit.\n";
        
        // Main loop
        while (!window.should_close()) {
            // Calculate delta time
            auto current_time = std::chrono::high_resolution_clock::now();
            float dt = std::chrono::duration<float>(current_time - last_time).count();
            last_time = current_time;
            
            // Update FPS counter
            frame_count++;
            if (frame_count >= 60) {
                fps = 1.0f / dt;
                frame_count = 0;
                
                std::wstring title = L"ZWidget Rendering Demo - FPS: " + 
                                     std::to_wstring(static_cast<int>(fps));
                window.set_title(title);
            }
            
            // Update
            scene.update(dt);
            
            // Process events
            window.poll_events();
            
            // Render
            {
                DrawScope draw(render_ctx);
                
                // Clear
                canvas.clear(Color::white());
                
                // Draw demo scene
                scene.draw_shapes(canvas);
                
                // Draw widgets
                root->render(canvas);
            }
            
            // Present
            render_ctx.present(1);  // VSync
        }
        
        std::wcout << L"\nExiting demo...\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}