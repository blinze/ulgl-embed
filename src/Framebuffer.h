#pragma once

#include <glad/glad.h>
#include <cstdint>

class Framebuffer
{
private:
    uint32_t m_RendererID;
    uint32_t m_ColorAttachment;
    uint32_t m_Width;
    uint32_t m_Height;

public:
    Framebuffer(uint32_t width, uint32_t height);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    void Bind() const;
    void Unbind() const;

    void Resize(uint32_t width, uint32_t height);

    uint32_t GetID() const { return m_RendererID; }
    uint32_t GetColorAttachment() const { return m_ColorAttachment; }
    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

    void BindColorAttachment(uint32_t slot = 0) const;
};
