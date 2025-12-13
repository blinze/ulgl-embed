#include "UltralightRenderer.h"
#include <iostream>

UltralightRenderer::UltralightRenderer()
    : m_Initialized(false)
{
}

UltralightRenderer::~UltralightRenderer()
{
    Shutdown();
}

UltralightRenderer::UltralightRenderer(UltralightRenderer&& other) noexcept
    : m_Renderer(std::move(other.m_Renderer))
    , m_View(std::move(other.m_View))
    , m_Initialized(other.m_Initialized)
{
    other.m_Initialized = false;
}

UltralightRenderer& UltralightRenderer::operator=(UltralightRenderer&& other) noexcept
{
    if (this != &other)
    {
        Shutdown();
        m_Renderer = std::move(other.m_Renderer);
        m_View = std::move(other.m_View);
        m_Initialized = other.m_Initialized;
        other.m_Initialized = false;
    }
    return *this;
}

bool UltralightRenderer::Initialize(uint32_t width, uint32_t height)
{
    if (m_Initialized)
    {
        std::cerr << "UltralightRenderer already initialized!" << std::endl;
        return false;
    }

    try
    {
        ultralight::Config config;
        config.user_stylesheet = "";

        ultralight::Platform::instance().set_config(config);
        ultralight::Platform::instance().set_font_loader(ultralight::GetPlatformFontLoader());
        ultralight::Platform::instance().set_file_system(ultralight::GetPlatformFileSystem("."));
        ultralight::Platform::instance().set_logger(ultralight::GetDefaultLogger("ultralight.log"));

        m_Renderer = ultralight::Renderer::Create();

        ultralight::ViewConfig view_config;
        view_config.is_accelerated = false;
        view_config.initial_device_scale = 1.0;
        view_config.is_transparent = false;
        view_config.font_family_standard = "Arial";
        view_config.font_family_serif = "Times New Roman";
        view_config.font_family_sans_serif = "Arial";
        view_config.font_family_fixed = "Courier New";

        m_View = m_Renderer->CreateView(width, height, view_config, nullptr);

        m_Initialized = true;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to initialize Ultralight: " << e.what() << std::endl;
        return false;
    }
}

void UltralightRenderer::Shutdown()
{
    if (!m_Initialized)
        return;

    m_View = nullptr;
    m_Renderer = nullptr;
    m_Initialized = false;
}

void UltralightRenderer::Update()
{
    if (!m_Initialized || !m_Renderer)
        return;

    m_Renderer->Update();
}

void UltralightRenderer::Render()
{
    if (!m_Initialized || !m_Renderer || !m_View)
        return;

    // Render all active views (RefreshDisplay doesn't exist in this API version)
    m_Renderer->Render();
}

void UltralightRenderer::LoadHTML(const std::string& html)
{
    if (!m_Initialized || !m_View)
        return;

    m_View->LoadHTML(html.c_str());
}

void UltralightRenderer::LoadURL(const std::string& url)
{
    if (!m_Initialized || !m_View)
        return;

    m_View->LoadURL(url.c_str());
}

ultralight::Bitmap* UltralightRenderer::GetBitmap() const
{
    if (!m_Initialized || !m_View)
        return nullptr;

    ultralight::Surface* surface = m_View->surface();
    if (!surface)
        return nullptr;

    ultralight::BitmapSurface* bitmap_surface = static_cast<ultralight::BitmapSurface*>(surface);
    if (!bitmap_surface)
        return nullptr;

    return bitmap_surface->bitmap().get();
}

bool UltralightRenderer::IsDirty() const
{
    if (!m_Initialized || !m_View)
        return false;

    ultralight::Surface* surface = m_View->surface();
    if (!surface)
        return false;

    return !surface->dirty_bounds().IsEmpty();
}

void UltralightRenderer::ClearDirty()
{
    if (!m_Initialized || !m_View)
        return;

    ultralight::Surface* surface = m_View->surface();
    if (!surface)
        return;

    ultralight::BitmapSurface* bitmap_surface = static_cast<ultralight::BitmapSurface*>(surface);
    if (bitmap_surface)
    {
        bitmap_surface->ClearDirtyBounds();
    }
}