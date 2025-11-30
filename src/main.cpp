/**
 * @file widget_demo.cpp
 * @brief Comprehensive demo showing all implemented widgets and layouts
 */

#include "zwidget/core/window.hpp"
#include "zwidget/core/widget.hpp"
#include "zwidget/widgets/label.hpp"
#include "zwidget/widgets/textbox.hpp"
#include "zwidget/widgets/checkbox.hpp"
#include "zwidget/widgets/button.hpp"
#include "zwidget/widgets/layout.hpp"
#include "zwidget/render/d2d/context.hpp"
#include "zwidget/render/canvas.hpp"
#include <iostream>

using namespace zuu::widget;

/**
 * @brief Create a form demo panel
 */
WidgetPtr create_form_panel() {
    auto panel = make_vbox(10.0f, 15.0f);
    panel->set_background(Color(250, 250, 250, 255));
    panel->set_bounds(Rectf{10, 10, 380, 400});
    
    // Title
    auto title = make_label(L"User Registration Form");
    TextStyle title_style;
    title_style.font_size = 18.0f;
    title_style.bold = true;
    static_cast<Label*>(title.get())->set_text_style(title_style);
    title->set_preferred_size(Sizef{350, 30});
    panel->add_child(title);
    
    // Name field
    auto name_row = make_hbox(10.0f);
    name_row->set_preferred_size(Sizef{350, 30});
    
    auto name_label = make_label(L"Name:");
    name_label->set_preferred_size(Sizef{100, 30});
    name_row->add_child(name_label);
    
    auto name_input = make_textbox();
    static_cast<TextBox*>(name_input.get())->set_placeholder(L"Enter your name");
    name_input->set_preferred_size(Sizef{240, 30});
    name_row->add_child(name_input);
    
    panel->add_child(name_row);
    
    // Email field
    auto email_row = make_hbox(10.0f);
    email_row->set_preferred_size(Sizef{350, 30});
    
    auto email_label = make_label(L"Email:");
    email_label->set_preferred_size(Sizef{100, 30});
    email_row->add_child(email_label);
    
    auto email_input = make_textbox();
    static_cast<TextBox*>(email_input.get())->set_placeholder(L"user@example.com");
    static_cast<TextBox*>(email_input.get())->set_on_text_changed([](const std::wstring& text) {
        std::wcout << L"Email changed: " << text << L"\n";
    });
    email_input->set_preferred_size(Sizef{240, 30});
    email_row->add_child(email_input);
    
    panel->add_child(email_row);
    
    // Checkboxes
    auto checkbox1 = make_checkbox(L"Subscribe to newsletter", false);
    checkbox1->set_preferred_size(Sizef{350, 24});
    static_cast<CheckBox*>(checkbox1.get())->set_on_checked_changed([](bool checked) {
        std::wcout << L"Newsletter: " << (checked ? L"Yes" : L"No") << L"\n";
    });
    panel->add_child(checkbox1);
    
    auto checkbox2 = make_checkbox(L"I agree to terms and conditions", false);
    checkbox2->set_preferred_size(Sizef{350, 24});
    panel->add_child(checkbox2);
    
    auto checkbox3 = make_checkbox(L"Enable notifications", true);
    checkbox3->set_preferred_size(Sizef{350, 24});
    panel->add_child(checkbox3);
    
    // Buttons row
    auto button_row = make_hbox(10.0f);
    button_row->set_preferred_size(Sizef{350, 40});
    button_row->set_alignment(LayoutAlign::end);
    
    auto cancel_btn = std::make_shared<Button>(L"Cancel");
    cancel_btn->set_preferred_size(Sizef{100, 35});
    cancel_btn->set_background(Color(240, 240, 240, 255));
    button_row->add_child(cancel_btn);
    
    auto submit_btn = std::make_shared<Button>(L"Submit");
    submit_btn->set_preferred_size(Sizef{100, 35});
    submit_btn->set_background(Color(0, 120, 215, 255));
    submit_btn->set_foreground(Color::white());
    button_row->add_child(submit_btn);
    
    panel->add_child(button_row);
    
    return panel;
}

/**
 * @brief Create a settings panel
 */
WidgetPtr create_settings_panel() {
    auto panel = make_vbox(8.0f, 15.0f);
    panel->set_background(Color(245, 245, 245, 255));
    panel->set_bounds(Rectf{410, 10, 380, 400});
    
    // Title
    auto title = make_label(L"Application Settings");
    TextStyle title_style;
    title_style.font_size = 18.0f;
    title_style.bold = true;
    static_cast<Label*>(title.get())->set_text_style(title_style);
    title->set_preferred_size(Sizef{350, 30});
    panel->add_child(title);
    
    // Section: Display
    auto display_label = make_label(L"Display Options");
    TextStyle section_style;
    section_style.font_size = 14.0f;
    section_style.bold = true;
    static_cast<Label*>(display_label.get())->set_text_style(section_style);
    display_label->set_preferred_size(Sizef{350, 25});
    panel->add_child(display_label);
    
    auto dark_mode = make_checkbox(L"Enable dark mode", false);
    dark_mode->set_preferred_size(Sizef{350, 24});
    panel->add_child(dark_mode);
    
    auto fullscreen = make_checkbox(L"Fullscreen mode", false);
    fullscreen->set_preferred_size(Sizef{350, 24});
    panel->add_child(fullscreen);
    
    // Section: Privacy
    auto privacy_label = make_label(L"Privacy");
    static_cast<Label*>(privacy_label.get())->set_text_style(section_style);
    privacy_label->set_preferred_size(Sizef{350, 25});
    panel->add_child(privacy_label);
    
    auto analytics = make_checkbox(L"Send usage statistics", true);
    analytics->set_preferred_size(Sizef{350, 24});
    static_cast<CheckBox*>(analytics.get())->set_tristate(true);
    panel->add_child(analytics);
    
    auto crash_reports = make_checkbox(L"Send crash reports", true);
    crash_reports->set_preferred_size(Sizef{350, 24});
    panel->add_child(crash_reports);
    
    // Section: Advanced
    auto advanced_label = make_label(L"Advanced");
    static_cast<Label*>(advanced_label.get())->set_text_style(section_style);
    advanced_label->set_preferred_size(Sizef{350, 25});
    panel->add_child(advanced_label);
    
    auto auto_update = make_checkbox(L"Automatic updates", true);
    auto_update->set_preferred_size(Sizef{350, 24});
    panel->add_child(auto_update);
    
    auto experimental = make_checkbox(L"Enable experimental features", false);
    experimental->set_preferred_size(Sizef{350, 24});
    panel->add_child(experimental);
    
    // Save button
    auto save_btn = std::make_shared<Button>(L"Save Settings");
    save_btn->set_preferred_size(Sizef{150, 35});
    save_btn->set_background(Color(40, 180, 80, 255));
    save_btn->set_foreground(Color::white());
    
    auto button_container = make_hbox();
    button_container->set_preferred_size(Sizef{350, 40});
    button_container->set_alignment(LayoutAlign::center);
    button_container->add_child(save_btn);
    
    panel->add_child(button_container);
    
    return panel;
}

/**
 * @brief Create info panel
 */
WidgetPtr create_info_panel() {
    auto panel = make_vbox(5.0f, 15.0f);
    panel->set_background(Color(255, 250, 240, 255));
    panel->set_bounds(Rectf{10, 430, 780, 150});
    
    auto info_title = make_label(L"💡 Quick Tips");
    TextStyle tip_style;
    tip_style.font_size = 16.0f;
    tip_style.bold = true;
    static_cast<Label*>(info_title.get())->set_text_style(tip_style);
    info_title->set_preferred_size(Sizef{750, 25});
    panel->add_child(info_title);
    
    auto tip1 = make_label(L"• Use TAB to navigate between form fields");
    tip1->set_preferred_size(Sizef{750, 20});
    panel->add_child(tip1);
    
    auto tip2 = make_label(L"• Press SPACE to toggle checkboxes when focused");
    tip2->set_preferred_size(Sizef{750, 20});
    panel->add_child(tip2);
    
    auto tip3 = make_label(L"• Click and drag to select text in input fields");
    tip3->set_preferred_size(Sizef{750, 20});
    panel->add_child(tip3);
    
    auto tip4 = make_label(L"• Press ESC to close the application");
    tip4->set_preferred_size(Sizef{750, 20});
    panel->add_child(tip4);
    
    return panel;
}

int main() {
    try {
        std::wcout << L"=== ZWidget Complete Demo ===\n\n";
        std::wcout << L"Demonstrating:\n";
        std::wcout << L"  ✓ Labels\n";
        std::wcout << L"  ✓ TextBoxes with placeholder and callbacks\n";
        std::wcout << L"  ✓ CheckBoxes (including tristate)\n";
        std::wcout << L"  ✓ Buttons with custom colors\n";
        std::wcout << L"  ✓ HBox and VBox layouts\n";
        std::wcout << L"  ✓ Layout alignment and spacing\n\n";
        
        // Create window
        WindowConfig config;
        config.title = L"ZWidget Demo - Widgets & Layouts";
        config.size = Size{820, 620};
        
        Window window(config);
        
        // Create render context
        D2DContext render_ctx(window.native_handle());
        Canvas canvas(render_ctx);
        
        // Create root container
        auto root = make_widget<Widget>();
        root->set_bounds(Rectf{0, 0, 820, 620});
        root->set_background(Color(240, 240, 240, 255));
        
        // Add panels
        root->add_child(create_form_panel());
        root->add_child(create_settings_panel());
        root->add_child(create_info_panel());
        
        // Perform initial layout
        root->layout();
        
        // Event handling
        Widget* hovered_widget = nullptr;
        Widget* focused_widget = nullptr;
        
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
                        root->layout();
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
                    
                    // Handle focus on click
                    if (evt.state == mouse_state::press && 
                        evt.button == mouse_button::left && hit) {
                        
                        if (focused_widget != hit) {
                            if (focused_widget) {
                                focused_widget->set_focused(false);
                            }
                            focused_widget = hit;
                            focused_widget->set_focused(true);
                        }
                    }
                    
                    // Dispatch to widget
                    if (hit) {
                        hit->on_mouse_event(evt);
                    }
                }
                else if constexpr (std::is_same_v<T, KeyboardEvent>) {
                    if (evt.state == keyboard_state::press) {
                        if (evt.key_code == VK_ESCAPE) {
                            window.set_should_close(true);
                        }
                        
                        // Dispatch to focused widget
                        if (focused_widget) {
                            focused_widget->on_keyboard_event(evt);
                        }
                    }
                }
            });
        });
        
        std::wcout << L"Window created. Interact with the widgets!\n";
        std::wcout << L"Press ESC to exit.\n\n";
        
        // Main loop
        while (!window.should_close()) {
            // Process events
            window.poll_events();
            
            // Render
            {
                DrawScope draw(render_ctx);
                canvas.clear(Color(240, 240, 240, 255));
                root->render(canvas);
            }
            
            render_ctx.present(1);
        }
        
        std::wcout << L"\nDemo completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}