#pragma once

#include <glad/glad.h>
#include <cstdint>
#include <string>

class Texture
{
private:
    uint32_t m_RendererID;
    uint32_t m_Width;
    uint32_t m_Height;
    uint32_t m_InternalFormat;
    uint32_t m_DataFormat;
public:
    Texture();
    Texture(uint32_t width, uint32_t height, const void* data = nullptr, 
            uint32_t internalFormat = GL_RGBA8, uint32_t dataFormat = GL_BGRA);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void Bind(uint32_t slot = 0) const;
    void Unbind() const;

    void UpdateData(const void* data, uint32_t width, uint32_t height);
    void Resize(uint32_t width, uint32_t height);

    uint32_t GetID() const { return m_RendererID; }
    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }
};
