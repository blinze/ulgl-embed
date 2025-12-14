#pragma once

#include <Ultralight/Ultralight.h>
#include <Ultralight/Listener.h>
#include <AppCore/Platform.h>
#include <cstdint>
#include <string>
#include <functional>
#include <unordered_map>

struct ComponentSlot
{
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
    bool visible = false;
};

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
    void OnAddConsoleMessage(ultralight::View* caller,
                            const ultralight::ConsoleMessage& message) override;
};

class UltralightRenderer
{
private:
    ultralight::RefPtr<ultralight::Renderer> m_Renderer;
    ultralight::RefPtr<ultralight::View> m_View;
    bool m_Initialized;
    uint32_t m_Width;
    uint32_t m_Height;
    UltralightLoadListener m_LoadListener;
    UltralightViewListener m_ViewListener;
    std::unordered_map<std::string, ComponentSlot> m_ComponentSlots;

public:
    void BindJavaScriptAPI();
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
    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

    ultralight::Bitmap* GetBitmap() const;
    bool IsDirty() const;
    void ClearDirty();
    void ForceRepaint();

    void SetComponentSlot(const std::string& name, float x, float y, float width, float height, bool visible);
    const ComponentSlot* GetComponentSlot(const std::string& name) const;
    const std::unordered_map<std::string, ComponentSlot>& GetAllSlots() const { return m_ComponentSlots; }
};
