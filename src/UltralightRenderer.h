#pragma once

#include <Ultralight/Ultralight.h>
#include <Ultralight/Listener.h>
#include <AppCore/Platform.h>
#include <cstdint>
#include <string>

class UltralightLoadListener : public ultralight::LoadListener
{
public:
    void OnBeginLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,
                        const ultralight::String& url) override;
    void OnFinishLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,
                         const ultralight::String& url) override;
    void OnFailLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,
                       const ultralight::String& url, const ultralight::String& description,
                       const ultralight::String& error_domain, int error_code) override;
    void OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,
                    const ultralight::String& url) override;
};

class UltralightViewListener : public ultralight::ViewListener
{
public:
    void OnAddConsoleMessage(ultralight::View* caller, ultralight::MessageSource source,
                            ultralight::MessageLevel level, const ultralight::String& message,
                            uint32_t line_number, uint32_t column_number,
                            const ultralight::String& source_id) override;
};

class UltralightRenderer
{
private:
    ultralight::RefPtr<ultralight::Renderer> m_Renderer;
    ultralight::RefPtr<ultralight::View> m_View;
    bool m_Initialized;
    UltralightLoadListener m_LoadListener;
    UltralightViewListener m_ViewListener;
public:
    UltralightRenderer();
    ~UltralightRenderer();

    UltralightRenderer(const UltralightRenderer&) = delete;
    UltralightRenderer& operator=(const UltralightRenderer&) = delete;

    UltralightRenderer(UltralightRenderer&& other) noexcept;
    UltralightRenderer& operator=(UltralightRenderer&& other) noexcept;

    bool Initialize(uint32_t width, uint32_t height);
    void Shutdown();

    void Update();
    void Render();

    void LoadHTML(const std::string& html);
    void LoadURL(const std::string& url);

    ultralight::View* GetView() const { return m_View.get(); }
    bool IsInitialized() const { return m_Initialized; }

    ultralight::Bitmap* GetBitmap() const;
    bool IsDirty() const;
    void ClearDirty();
    void ForceRepaint();
};