#include "zwidget/render/d2d/context.hpp"
#include <stdexcept>
#include <cmath>

namespace zuu::widget {

// === D2DFactory Implementation ===

D2DFactory* D2DFactory::instance_ = nullptr;
std::once_flag D2DFactory::init_flag_;

D2DFactory::D2DFactory() {
    HRESULT hr;
    
    // Create D2D factory (multi-threaded)
    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_MULTI_THREADED,
        __uuidof(ID2D1Factory1),
        reinterpret_cast<void**>(factory_.GetAddressOf())
    );
    
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create D2D factory");
    }
    
    // Create DWrite factory
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(write_factory_.GetAddressOf())
    );
    
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create DWrite factory");
    }
}

// === D2DContext Implementation ===

D2DContext::D2DContext(HWND hwnd) {
    std::lock_guard lock(mutex_);
    
    // Get DPI
    HDC hdc = GetDC(hwnd);
    dpi_scale_ = GetDeviceCaps(hdc, LOGPIXELSX) / 96.0f;
    ReleaseDC(hwnd, hdc);
    
    // Get window size
    RECT rect;
    GetClientRect(hwnd, &rect);
    size_ = Size{rect.right - rect.left, rect.bottom - rect.top};
    
    create_device_resources();
    create_swap_chain(hwnd);
    create_render_target();
    
    dwrite_factory_ = D2DFactory::get().write_factory();
}

D2DContext::~D2DContext() {
    std::lock_guard lock(mutex_);
    release_device_resources();
}

void D2DContext::create_device_resources() {
    HRESULT hr;
    
    // Create D3D11 device
    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    
    hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        feature_levels,
        ARRAYSIZE(feature_levels),
        D3D11_SDK_VERSION,
        &d3d_device_,
        nullptr,
        &d3d_context_
    );
    
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create D3D11 device");
    }
    
    // Create D2D device
    ComPtr<IDXGIDevice> dxgi_device;
    hr = d3d_device_.As(&dxgi_device);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to get DXGI device");
    }
    
    hr = D2DFactory::get().factory()->CreateDevice(
        dxgi_device.Get(),
        &d2d_device_
    );
    
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create D2D device");
    }
    
    // Create D2D device context
    hr = d2d_device_->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        &d2d_context_
    );
    
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create D2D device context");
    }
}

void D2DContext::create_swap_chain(HWND hwnd) {
    HRESULT hr;
    
    // Get DXGI factory
    ComPtr<IDXGIDevice> dxgi_device;
    ComPtr<IDXGIAdapter> adapter;
    ComPtr<IDXGIFactory2> dxgi_factory;
    
    hr = d3d_device_.As(&dxgi_device);
    if (FAILED(hr)) return;
    
    hr = dxgi_device->GetAdapter(&adapter);
    if (FAILED(hr)) return;
    
    hr = adapter->GetParent(__uuidof(IDXGIFactory2), &dxgi_factory);
    if (FAILED(hr)) return;
    
    // Create swap chain
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = size_.w;
    desc.Height = size_.h;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.Flags = 0;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    
    hr = dxgi_factory->CreateSwapChainForHwnd(
        d3d_device_.Get(),
        hwnd,
        &desc,
        nullptr,
        nullptr,
        &swap_chain_
    );
    
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create swap chain");
    }
    
    // Disable Alt+Enter fullscreen
    dxgi_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
}

void D2DContext::create_render_target() {
    HRESULT hr;
    
    // Get back buffer
    ComPtr<IDXGISurface> surface;
    hr = swap_chain_->GetBuffer(0, __uuidof(IDXGISurface), &surface);
    if (FAILED(hr)) return;
    
    // Create bitmap
    D2D1_BITMAP_PROPERTIES1 props = {};
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    props.dpiX = 96.0f * dpi_scale_;
    props.dpiY = 96.0f * dpi_scale_;
    
    hr = d2d_context_->CreateBitmapFromDxgiSurface(
        surface.Get(),
        &props,
        &target_bitmap_
    );
    
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create render target");
    }
    
    d2d_context_->SetTarget(target_bitmap_.Get());
    d2d_context_->SetDpi(96.0f * dpi_scale_, 96.0f * dpi_scale_);
}

void D2DContext::release_device_resources() {
    target_bitmap_.Reset();
    d2d_context_.Reset();
    d2d_device_.Reset();
    swap_chain_.Reset();
    d3d_context_.Reset();
    d3d_device_.Reset();
}

void D2DContext::begin_draw() {
    std::lock_guard lock(mutex_);
    
    if (is_drawing_) {
        throw std::runtime_error("Already drawing");
    }
    
    d2d_context_->BeginDraw();
    is_drawing_ = true;
}

void D2DContext::end_draw() {
    std::lock_guard lock(mutex_);
    
    if (!is_drawing_) return;
    
    HRESULT hr = d2d_context_->EndDraw();
    is_drawing_ = false;
    
    if (hr == D2DERR_RECREATE_TARGET) {
        handle_device_lost();
    }
}

void D2DContext::clear(const Color& color) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    d2d_context_->Clear(to_d2d_color(color));
}

void D2DContext::save_state() {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    TransformState state;
    d2d_context_->GetTransform(&state.transform);
    
    state_stack_.push(state);
}

void D2DContext::restore_state() {
    std::lock_guard lock(mutex_);
    if (!is_drawing_ || state_stack_.empty()) return;
    
    auto state = state_stack_.top();
    state_stack_.pop();
    
    d2d_context_->SetTransform(state.transform);
    
    if (state.has_clip) {
        d2d_context_->PopAxisAlignedClip();
    }
}

void D2DContext::translate(float x, float y) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    D2D1_MATRIX_3X2_F transform;
    d2d_context_->GetTransform(&transform);
    transform = transform * D2D1::Matrix3x2F::Translation(x, y);
    d2d_context_->SetTransform(transform);
}

void D2DContext::scale(float sx, float sy) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    D2D1_MATRIX_3X2_F transform;
    d2d_context_->GetTransform(&transform);
    transform = transform * D2D1::Matrix3x2F::Scale(sx, sy);
    d2d_context_->SetTransform(transform);
}

void D2DContext::rotate(float radians) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    float degrees = radians * 180.0f / 3.14159265f;
    D2D1_MATRIX_3X2_F transform;
    d2d_context_->GetTransform(&transform);
    transform = transform * D2D1::Matrix3x2F::Rotation(degrees);
    d2d_context_->SetTransform(transform);
}

void D2DContext::reset_transform() {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    d2d_context_->SetTransform(D2D1::Matrix3x2F::Identity());
}

void D2DContext::set_clip_rect(const Rectf& rect) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    d2d_context_->PushAxisAlignedClip(
        to_d2d_rect(rect),
        D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
    );
}

void D2DContext::reset_clip() {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    d2d_context_->PopAxisAlignedClip();
}

void D2DContext::draw_line(
    const Pointf& start,
    const Pointf& end,
    const Color& color,
    float width
) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    auto* brush = get_solid_brush(color);
    d2d_context_->DrawLine(
        to_d2d_point(start),
        to_d2d_point(end),
        brush,
        width
    );
}

void D2DContext::draw_rect(const Rectf& rect, const Color& color, float width) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    auto* brush = get_solid_brush(color);
    d2d_context_->DrawRectangle(to_d2d_rect(rect), brush, width);
}

void D2DContext::fill_rect(const Rectf& rect, const Color& color) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    auto* brush = get_solid_brush(color);
    d2d_context_->FillRectangle(to_d2d_rect(rect), brush);
}

void D2DContext::draw_rounded_rect(
    const Rectf& rect,
    float radius_x,
    float radius_y,
    const Color& color,
    float width
) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    D2D1_ROUNDED_RECT rounded = {
        to_d2d_rect(rect),
        radius_x,
        radius_y
    };
    
    auto* brush = get_solid_brush(color);
    d2d_context_->DrawRoundedRectangle(rounded, brush, width);
}

void D2DContext::fill_rounded_rect(
    const Rectf& rect,
    float radius_x,
    float radius_y,
    const Color& color
) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    D2D1_ROUNDED_RECT rounded = {
        to_d2d_rect(rect),
        radius_x,
        radius_y
    };
    
    auto* brush = get_solid_brush(color);
    d2d_context_->FillRoundedRectangle(rounded, brush);
}

void D2DContext::draw_ellipse(
    const Pointf& center,
    float radius_x,
    float radius_y,
    const Color& color,
    float width
) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    auto* brush = get_solid_brush(color);
    d2d_context_->DrawEllipse(
        to_d2d_ellipse(center, radius_x, radius_y),
        brush,
        width
    );
}

void D2DContext::fill_ellipse(
    const Pointf& center,
    float radius_x,
    float radius_y,
    const Color& color
) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    auto* brush = get_solid_brush(color);
    d2d_context_->FillEllipse(
        to_d2d_ellipse(center, radius_x, radius_y),
        brush
    );
}

void D2DContext::draw_polyline(
    const std::vector<Pointf>& points,
    const Color& color,
    float width,
    bool closed
) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_ || points.size() < 2) return;
    
    // Create geometry
    ComPtr<ID2D1PathGeometry> geometry;
    D2DFactory::get().factory()->CreatePathGeometry(&geometry);
    
    ComPtr<ID2D1GeometrySink> sink;
    geometry->Open(&sink);
    
    sink->BeginFigure(
        to_d2d_point(points[0]),
        D2D1_FIGURE_BEGIN_HOLLOW
    );
    
    for (size_t i = 1; i < points.size(); ++i) {
        sink->AddLine(to_d2d_point(points[i]));
    }
    
    sink->EndFigure(closed ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
    sink->Close();
    
    auto* brush = get_solid_brush(color);
    d2d_context_->DrawGeometry(geometry.Get(), brush, width);
}

void D2DContext::fill_polygon(
    const std::vector<Pointf>& points,
    const Color& color
) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_ || points.size() < 3) return;
    
    // Create geometry
    ComPtr<ID2D1PathGeometry> geometry;
    D2DFactory::get().factory()->CreatePathGeometry(&geometry);
    
    ComPtr<ID2D1GeometrySink> sink;
    geometry->Open(&sink);
    
    sink->BeginFigure(
        to_d2d_point(points[0]),
        D2D1_FIGURE_BEGIN_FILLED
    );
    
    for (size_t i = 1; i < points.size(); ++i) {
        sink->AddLine(to_d2d_point(points[i]));
    }
    
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    
    auto* brush = get_solid_brush(color);
    d2d_context_->FillGeometry(geometry.Get(), brush);
}

void D2DContext::draw_text(
    const std::wstring& text,
    const Pointf& position,
    const Color& color,
    const TextStyle& style
) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    auto format = create_text_format(style);
    auto* brush = get_solid_brush(color);
    
    // Measure text to create rect
    auto size = measure_text(text, style);
    Rectf rect{position.x, position.y, size.w, size.h};
    
    d2d_context_->DrawText(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        format.Get(),
        to_d2d_rect(rect),
        brush
    );
}

void D2DContext::draw_text(
    const std::wstring& text,
    const Rectf& rect,
    const Color& color,
    const TextStyle& style
) {
    std::lock_guard lock(mutex_);
    if (!is_drawing_) return;
    
    auto format = create_text_format(style);
    auto* brush = get_solid_brush(color);
    
    d2d_context_->DrawText(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        format.Get(),
        to_d2d_rect(rect),
        brush
    );
}

Sizef D2DContext::measure_text(
    const std::wstring& text,
    const TextStyle& style
) {
    std::lock_guard lock(mutex_);
    
    auto format = create_text_format(style);
    
    ComPtr<IDWriteTextLayout> layout;
    dwrite_factory_->CreateTextLayout(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        format.Get(),
        FLT_MAX,
        FLT_MAX,
        &layout
    );
    
    DWRITE_TEXT_METRICS metrics;
    layout->GetMetrics(&metrics);
    
    return Sizef{metrics.width, metrics.height};
}

// Stub implementations for image and gradient methods
void D2DContext::draw_image(const Image&, const Pointf&, float) {}
void D2DContext::draw_image(const Image&, const Rectf&, float) {}
void D2DContext::draw_image(const Image&, const Rectf&, const Rectf&, float) {}
void D2DContext::fill_rect_gradient(const Rectf&, const Color&, const Color&, const Pointf&, const Pointf&) {}
void D2DContext::fill_rect_radial_gradient(const Rectf&, const Color&, const Color&, const Pointf&, float, float) {}

Sizef D2DContext::get_size() const {
    std::lock_guard lock(mutex_);
    return Sizef{static_cast<float>(size_.w), static_cast<float>(size_.h)};
}

void D2DContext::resize(const Size& new_size) {
    std::lock_guard lock(mutex_);
    
    if (is_drawing_) {
        throw std::runtime_error("Cannot resize while drawing");
    }
    
    size_ = new_size;
    
    target_bitmap_.Reset();
    
    HRESULT hr = swap_chain_->ResizeBuffers(
        2,
        new_size.w,
        new_size.h,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        0
    );
    
    if (SUCCEEDED(hr)) {
        create_render_target();
    }
}

void D2DContext::flush() {
    std::lock_guard lock(mutex_);
    if (d2d_context_) {
        d2d_context_->Flush();
    }
}

HRESULT D2DContext::present(UINT sync_interval) {
    std::lock_guard lock(mutex_);
    return swap_chain_->Present(sync_interval, 0);
}

void D2DContext::handle_device_lost() {
    release_device_resources();
    // Recreate resources (implementation depends on window handle storage)
}

// === Helper Methods ===

ID2D1SolidColorBrush* D2DContext::get_solid_brush(const Color& color) {
    if (!brush_cache_.solid_brush || brush_cache_.last_color != color) {
        brush_cache_.solid_brush.Reset();
        d2d_context_->CreateSolidColorBrush(
            to_d2d_color(color),
            &brush_cache_.solid_brush
        );
        brush_cache_.last_color = color;
    }
    return brush_cache_.solid_brush.Get();
}

ComPtr<IDWriteTextFormat> D2DContext::create_text_format(const TextStyle& style) {
    ComPtr<IDWriteTextFormat> format;
    
    DWRITE_FONT_WEIGHT weight = style.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL;
    DWRITE_FONT_STYLE font_style = style.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;
    
    dwrite_factory_->CreateTextFormat(
        style.font_family.c_str(),
        nullptr,
        weight,
        font_style,
        DWRITE_FONT_STRETCH_NORMAL,
        style.font_size,
        L"",
        &format
    );
    
    // Set alignment
    DWRITE_TEXT_ALIGNMENT text_align;
    switch (style.align) {
        case TextAlign::center: text_align = DWRITE_TEXT_ALIGNMENT_CENTER; break;
        case TextAlign::right: text_align = DWRITE_TEXT_ALIGNMENT_TRAILING; break;
        default: text_align = DWRITE_TEXT_ALIGNMENT_LEADING; break;
    }
    format->SetTextAlignment(text_align);
    
    DWRITE_PARAGRAPH_ALIGNMENT para_align;
    switch (style.valign) {
        case TextVAlign::middle: para_align = DWRITE_PARAGRAPH_ALIGNMENT_CENTER; break;
        case TextVAlign::bottom: para_align = DWRITE_PARAGRAPH_ALIGNMENT_FAR; break;
        default: para_align = DWRITE_PARAGRAPH_ALIGNMENT_NEAR; break;
    }
    format->SetParagraphAlignment(para_align);
    
    return format;
}

D2D1_COLOR_F D2DContext::to_d2d_color(const Color& color) {
    return D2D1::ColorF(
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    );
}

D2D1_RECT_F D2DContext::to_d2d_rect(const Rectf& rect) {
    return D2D1::RectF(
        rect.left(),
        rect.top(),
        rect.right(),
        rect.bottom()
    );
}

D2D1_POINT_2F D2DContext::to_d2d_point(const Pointf& point) {
    return D2D1::Point2F(point.x, point.y);
}

D2D1_ELLIPSE D2DContext::to_d2d_ellipse(const Pointf& center, float rx, float ry) {
    return D2D1::Ellipse(to_d2d_point(center), rx, ry);
}

} // namespace zuu::widget
