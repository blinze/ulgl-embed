#include "UltralightRenderer.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

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
        std::cout << "  Setting up Ultralight config..." << std::endl;
        ultralight::Config config;
        config.user_stylesheet = "";

        std::cout << "  Setting platform config..." << std::endl;
        ultralight::Platform::instance().set_config(config);
        
        std::cout << "  Setting font loader..." << std::endl;
        ultralight::Platform::instance().set_font_loader(ultralight::GetPlatformFontLoader());
        
        std::cout << "  Setting file system..." << std::endl;
        std::filesystem::path currentDir = std::filesystem::current_path();
        std::cout << "    Current working directory: " << currentDir << std::endl;
        
        std::filesystem::path resourcesPath = currentDir / "resources";
        if (std::filesystem::exists(resourcesPath))
        {
            std::cout << "    Resources folder found at: " << resourcesPath << std::endl;
        }
        else
        {
            std::cerr << "    WARNING: Resources folder not found at: " << resourcesPath << std::endl;
            std::cerr << "    Ultralight may fail to initialize without resources!" << std::endl;
        }
        
        std::string fileSystemBase = currentDir.string();
#ifdef _WIN32
        std::replace(fileSystemBase.begin(), fileSystemBase.end(), '\\', '/');
        std::cout << "    File system base (normalized): " << fileSystemBase << std::endl;
#endif
        ultralight::Platform::instance().set_file_system(ultralight::GetPlatformFileSystem(fileSystemBase.c_str()));
        
        std::cout << "  Setting logger..." << std::endl;
        ultralight::Platform::instance().set_logger(ultralight::GetDefaultLogger("ultralight.log"));
        std::cout << "    Logger will write to: ultralight.log (check this file for detailed errors)" << std::endl;

#ifdef _WIN32
        std::cout << "  Checking for required DLLs..." << std::endl;
        const char* requiredDLLs[] =
        {
            "UltralightCore.dll",
            "Ultralight.dll",
            "WebCore.dll",
            "AppCore.dll"
        };
        
        bool allDLLsFound = true;
        for (const char* dll : requiredDLLs)
        {
            HMODULE hModule = LoadLibraryA(dll);
            if (hModule == nullptr)
            {
                DWORD error = GetLastError();
                std::cerr << "    ERROR: " << dll << " failed to load (Error code: " << error << ")" << std::endl;
                if (error == 126)
                {
                    std::cerr << "      This usually means a dependency is missing (like Visual C++ runtime)" << std::endl;
                }
                allDLLsFound = false;
            }
            else
            {
                std::cout << "    Successfully loaded: " << dll << std::endl;
                FreeLibrary(hModule);
            }
        }
        
        if (!allDLLsFound)
        {
            std::cerr << "  CRITICAL: Some required DLLs failed to load!" << std::endl;
            std::cerr << "  Make sure:" << std::endl;
            std::cerr << "    1. All Ultralight DLLs are in the same directory as the executable" << std::endl;
            std::cerr << "    2. Visual C++ Redistributable is installed" << std::endl;
            std::cerr << "    3. DLL architecture (x64/x86) matches your build" << std::endl;
            std::cerr << "  Continuing anyway, but Renderer::Create() will likely fail..." << std::endl;
        }
#endif
        std::cout << "  Creating renderer..." << std::endl;
        std::cout << "    Calling Renderer::Create()..." << std::endl;
        std::cout << "    NOTE: If the program crashes here, check:" << std::endl;
        std::cout << "      1. ultralight.log file in the working directory" << std::endl;
#ifdef _WIN32
        std::cout << "      2. Windows Event Viewer for crash details" << std::endl;
        std::cout << "      3. Visual C++ Redistributable is installed" << std::endl;
        std::cout << "      4. DLL architecture (x64) matches your build" << std::endl;
#else
        std::cout << "      2. System logs for crash details" << std::endl;
        std::cout << "      3. Required shared libraries are installed" << std::endl;
        std::cout << "      4. Library architecture matches your build" << std::endl;
#endif
        
        std::cout.flush();
        std::cerr.flush();
        
        try
        {
            m_Renderer = ultralight::Renderer::Create();
            std::cout << "    Renderer::Create() returned successfully!" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "    EXCEPTION in Renderer::Create(): " << e.what() << std::endl;
            std::cerr.flush();
            return false;
        }
        catch (...)
        {
            std::cerr << "    UNKNOWN EXCEPTION in Renderer::Create()!" << std::endl;
            std::cerr.flush();
            return false;
        }
        
        if (!m_Renderer)
        {
            std::cerr << "  Failed to create Ultralight renderer (returned nullptr)!" << std::endl;
            std::cerr << "  This usually means:" << std::endl;
#ifdef _WIN32
            std::cerr << "    1. Missing Ultralight DLLs in the executable directory" << std::endl;
            std::cerr << "    2. Missing resources folder" << std::endl;
            std::cerr << "    3. Missing Visual C++ runtime dependencies" << std::endl;
#else
            std::cerr << "    1. Missing Ultralight shared libraries in the executable directory" << std::endl;
            std::cerr << "    2. Missing resources folder" << std::endl;
            std::cerr << "    3. Missing required system dependencies" << std::endl;
#endif
            return false;
        }
        std::cout << "    Renderer created successfully!" << std::endl;

        std::cout << "  Configuring view..." << std::endl;
        ultralight::ViewConfig view_config;
        view_config.is_accelerated = false;
        view_config.initial_device_scale = 1.0;
        view_config.is_transparent = false;
        view_config.font_family_standard = "Arial";
        view_config.font_family_serif = "Times New Roman";
        view_config.font_family_sans_serif = "Arial";
        view_config.font_family_fixed = "Courier New";

        std::cout << "  Creating view..." << std::endl;
        m_View = m_Renderer->CreateView(width, height, view_config, nullptr);
        if (!m_View)
        {
            std::cerr << "  Failed to create Ultralight view!" << std::endl;
            return false;
        }

        std::cout << "  Setting up view listeners..." << std::endl;
        m_View->set_load_listener(&m_LoadListener);
        m_View->set_view_listener(&m_ViewListener);

        m_Initialized = true;
        std::cout << "  Ultralight initialization complete!" << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to initialize Ultralight: " << e.what() << std::endl;
        return false;
    }
    catch (...)
    {
        std::cerr << "Failed to initialize Ultralight: Unknown exception!" << std::endl;
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
        bitmap_surface->ClearDirtyBounds();
}

void UltralightRenderer::ForceRepaint()
{
    if (!m_Initialized || !m_View)
        return;

    m_View->set_needs_paint(true);
}

void UltralightLoadListener::OnBeginLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,
                                            const ultralight::String& url)
{
    if (is_main_frame)
        std::cout << "[Load] Beginning to load: " << url.utf8().data() << std::endl;
}

void UltralightLoadListener::OnFinishLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,
                                             const ultralight::String& url)
{
    if (is_main_frame)
        std::cout << "[Load] Finished loading: " << url.utf8().data() << std::endl;
}

void UltralightLoadListener::OnFailLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,
                                           const ultralight::String& url, const ultralight::String& description,
                                           const ultralight::String& error_domain, int error_code)
{
    if (is_main_frame)
    {
        std::cerr << "[Load ERROR] Failed to load: " << url.utf8().data() << std::endl;
        std::cerr << "  Description: " << description.utf8().data() << std::endl;
        std::cerr << "  Error Domain: " << error_domain.utf8().data() << std::endl;
        std::cerr << "  Error Code: " << error_code << std::endl;
    }
}

void UltralightLoadListener::OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,
                                        const ultralight::String& url)
{
    if (is_main_frame)
    {
        std::cout << "[Load] DOM ready for: " << url.utf8().data() << std::endl;
        // Force a repaint after DOM is ready to ensure content is rendered
        caller->set_needs_paint(true);
    }
}

void UltralightViewListener::OnAddConsoleMessage(ultralight::View* caller, ultralight::MessageSource source,
                                                 ultralight::MessageLevel level, const ultralight::String& message,
                                                 uint32_t line_number, uint32_t column_number,
                                                 const ultralight::String& source_id)
{
    const char* levelStr = "LOG";
    switch (level)
    {
        case ultralight::kMessageLevel_Warning: levelStr = "WARNING"; break;
        case ultralight::kMessageLevel_Error: levelStr = "ERROR"; break;
        case ultralight::kMessageLevel_Debug: levelStr = "DEBUG"; break;
        case ultralight::kMessageLevel_Info: levelStr = "INFO"; break;
        default: break;
    }
    
    std::cout << "[Console " << levelStr << "] " << message.utf8().data();
    if (line_number > 0)
        std::cout << " (line " << line_number << ", col " << column_number << ")";
    if (!source_id.empty())
        std::cout << " in " << source_id.utf8().data();
    std::cout << std::endl;
}
