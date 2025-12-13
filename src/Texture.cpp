#include "Texture.h"

Texture::Texture()
    : m_RendererID(0), m_Width(0), m_Height(0),
      m_InternalFormat(GL_RGBA8), m_DataFormat(GL_BGRA)
{
    glGenTextures(1, &m_RendererID);
}

Texture::Texture(uint32_t width, uint32_t height, const void* data,
                 uint32_t internalFormat, uint32_t dataFormat)
    : m_Width(width), m_Height(height),
      m_InternalFormat(internalFormat), m_DataFormat(dataFormat)
{
    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, width, height, 0,
                 m_DataFormat, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_RendererID);
}

Texture::Texture(Texture&& other) noexcept
    : m_RendererID(other.m_RendererID)
    , m_Width(other.m_Width)
    , m_Height(other.m_Height)
    , m_InternalFormat(other.m_InternalFormat)
    , m_DataFormat(other.m_DataFormat)
{
    other.m_RendererID = 0;
    other.m_Width = 0;
    other.m_Height = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other)
    {
        glDeleteTextures(1, &m_RendererID);
        m_RendererID = other.m_RendererID;
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_InternalFormat = other.m_InternalFormat;
        m_DataFormat = other.m_DataFormat;
        other.m_RendererID = 0;
        other.m_Width = 0;
        other.m_Height = 0;
    }
    return *this;
}

void Texture::Bind(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::UpdateData(const void* data, uint32_t width, uint32_t height)
{
    if (width != m_Width || height != m_Height)
    {
        Resize(width, height);
    }

    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                    m_DataFormat, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Resize(uint32_t width, uint32_t height)
{
    m_Width = width;
    m_Height = height;

    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, width, height, 0,
                 m_DataFormat, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}
