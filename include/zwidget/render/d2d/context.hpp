#pragma once

/**
 * @file d2d_context.hpp
 * @brief Direct2D rendering context implementation
 * @version 1.0
 * @date 2025-11-29
 */

#include "zwidget/render/context.hpp"
#include <d2d1_1.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <mutex>
#include <stack>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwrite.lib")

namespace zuu::widget {

using Microsoft::WRL::ComPtr;

/**
 * @brief Direct2D factory wrapper (singleton, thread-safe)
 */
class D2DFactory {
private:
    ComPtr<ID2D1Factory1> factory_;
    ComPtr<IDWriteFactory> write_factory_;
    std::mutex mutex_;
    
    static D2DFactory* instance_;
    static std::once_flag init_flag_;
    
    D2DFactory();
    
public:
    static D2DFactory& get() {
        std::call_once(init_flag_, []() {
            instance_ = new D2DFactory();
        });
        return *instance_;
    }
    
    ID2D1Factory1* factory() const { return factory_.Get(); }
    IDWriteFactory* write_factory() const { return write_factory_.Get(); }
    
    ~D2DFactory() = default;
    D2DFactory(const D2DFactory&) = delete;
    D2DFactory& operator=(const D2DFactory&) = delete;
};

/**
 * @brief Transform state for save/restore
 */
struct TransformState {
    D2D1_MATRIX_3X2_F transform;
    ComPtr<ID2D1Layer> clip_layer;
    D2D1_RECT_F clip_rect;
    bool has_clip = false;
};

/**
 * @brief Direct2D rendering context - thread-safe
 */
class D2DContext : public RenderContext {
private:
    // D3D11 and DXGI
    ComPtr<ID3D11Device> d3d_device_;
    ComPtr<ID3D11DeviceContext> d3d_context_;
    ComPtr<IDXGISwapChain1> swap_chain_;
    
    // D2D
    ComPtr<ID2D1Device> d2d_device_;
    ComPtr<ID2D1DeviceContext> d2d_context_;
    ComPtr<ID2D1Bitmap1> target_bitmap_;
    
    // DWrite
    ComPtr<IDWriteFactory> dwrite_factory_;
    
    // State
    std::stack<TransformState> state_stack_;
    bool is_drawing_ = false;
    float dpi_scale_ = 1.0f;
    Size size_;
    
    // Thread safety
    mutable std::recursive_mutex mutex_;
    
    // Resource cache
    struct BrushCache {
        ComPtr<ID2D1SolidColorBrush> solid_brush;
        Color last_color;
    } brush_cache_;
    
    // Helper methods
    void create_device_resources();
    void create_swap_chain(HWND hwnd);
    void create_render_target();
    void release_device_resources();
    
    ID2D1SolidColorBrush* get_solid_brush(const Color& color);
    ComPtr<IDWriteTextFormat> create_text_format(const TextStyle& style);
    D2D1_COLOR_F to_d2d_color(const Color& color);
    D2D1_RECT_F to_d2d_rect(const Rectf& rect);
    D2D1_POINT_2F to_d2d_point(const Pointf& point);
    D2D1_ELLIPSE to_d2d_ellipse(const Pointf& center, float rx, float ry);
    
public:
    /**
     * @brief Create Direct2D context for window
     */
    explicit D2DContext(HWND hwnd);
    ~D2DContext() override;
    
    // Non-copyable, movable
    D2DContext(const D2DContext&) = delete;
    D2DContext& operator=(const D2DContext&) = delete;
    D2DContext(D2DContext&&) noexcept = default;
    D2DContext& operator=(D2DContext&&) noexcept = default;
    
    // === RenderContext Interface ===
    
    void begin_draw() override;
    void end_draw() override;
    void clear(const Color& color) override;
    
    void save_state() override;
    void restore_state() override;
    
    void translate(float x, float y) override;
    void scale(float sx, float sy) override;
    void rotate(float radians) override;
    void reset_transform() override;
    
    void set_clip_rect(const Rectf& rect) override;
    void reset_clip() override;
    
    void draw_line(
        const Pointf& start,
        const Pointf& end,
        const Color& color,
        float width = 1.0f
    ) override;
    
    void draw_rect(
        const Rectf& rect,
        const Color& color,
        float width = 1.0f
    ) override;
    
    void fill_rect(
        const Rectf& rect,
        const Color& color
    ) override;
    
    void draw_rounded_rect(
        const Rectf& rect,
        float radius_x,
        float radius_y,
        const Color& color,
        float width = 1.0f
    ) override;
    
    void fill_rounded_rect(
        const Rectf& rect,
        float radius_x,
        float radius_y,
        const Color& color
    ) override;
    
    void draw_ellipse(
        const Pointf& center,
        float radius_x,
        float radius_y,
        const Color& color,
        float width = 1.0f
    ) override;
    
    void fill_ellipse(
        const Pointf& center,
        float radius_x,
        float radius_y,
        const Color& color
    ) override;
    
    void draw_polyline(
        const std::vector<Pointf>& points,
        const Color& color,
        float width = 1.0f,
        bool closed = false
    ) override;
    
    void fill_polygon(
        const std::vector<Pointf>& points,
        const Color& color
    ) override;
    
    void draw_text(
        const std::wstring& text,
        const Pointf& position,
        const Color& color,
        const TextStyle& style = TextStyle()
    ) override;
    
    void draw_text(
        const std::wstring& text,
        const Rectf& rect,
        const Color& color,
        const TextStyle& style = TextStyle()
    ) override;
    
    Sizef measure_text(
        const std::wstring& text,
        const TextStyle& style = TextStyle()
    ) override;
    
    void draw_image(
        const Image& image,
        const Pointf& position,
        float opacity = 1.0f
    ) override;
    
    void draw_image(
        const Image& image,
        const Rectf& dest_rect,
        float opacity = 1.0f
    ) override;
    
    void draw_image(
        const Image& image,
        const Rectf& dest_rect,
        const Rectf& source_rect,
        float opacity = 1.0f
    ) override;
    
    void fill_rect_gradient(
        const Rectf& rect,
        const Color& start_color,
        const Color& end_color,
        const Pointf& start_point,
        const Pointf& end_point
    ) override;
    
    void fill_rect_radial_gradient(
        const Rectf& rect,
        const Color& center_color,
        const Color& edge_color,
        const Pointf& center,
        float radius_x,
        float radius_y
    ) override;
    
    Sizef get_size() const override;
    float get_dpi_scale() const override { return dpi_scale_; }
    bool is_drawing() const override { return is_drawing_; }
    
    void resize(const Size& new_size) override;
    void flush() override;
    
    // === Direct2D Specific ===
    
    /**
     * @brief Get native D2D device context
     */
    ID2D1DeviceContext* native_context() const { return d2d_context_.Get(); }
    
    /**
     * @brief Get D3D11 device
     */
    ID3D11Device* d3d_device() const { return d3d_device_.Get(); }
    
    /**
     * @brief Present frame to screen
     */
    HRESULT present(UINT sync_interval = 1);
    
    /**
     * @brief Handle device lost
     */
    void handle_device_lost();
};

/**
 * @brief Image resource wrapper
 */
class Image {
private:
    ComPtr<ID2D1Bitmap> bitmap_;
    Sizef size_;
    
    friend class D2DContext;
    
public:
    Image() = default;
    
    /**
     * @brief Load image from file
     */
    static Image from_file(D2DContext& ctx, const std::wstring& path);
    
    /**
     * @brief Create image from memory
     */
    static Image from_memory(
        D2DContext& ctx,
        const void* data,
        size_t size,
        const Sizef& dimensions
    );
    
    /**
     * @brief Check if image is valid
     */
    bool is_valid() const { return bitmap_ != nullptr; }
    
    /**
     * @brief Get image size
     */
    Sizef size() const { return size_; }
    
    /**
     * @brief Get native D2D bitmap
     */
    ID2D1Bitmap* native_bitmap() const { return bitmap_.Get(); }
};

} // namespace zuu::widget
