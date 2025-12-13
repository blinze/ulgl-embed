#pragma once

#include <Ultralight/Ultralight.h>
#include <AppCore/Platform.h>
#include <cstdint>
#include <string>

class UltralightRenderer
{
private:
    ultralight::RefPtr<ultralight::Renderer> m_Renderer;
    ultralight::RefPtr<ultralight::View> m_View;
    bool m_Initialized;
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
};