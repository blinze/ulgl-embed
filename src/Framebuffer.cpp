#include "Framebuffer.h"
#include <iostream>

Framebuffer::Framebuffer(uint32_t width, uint32_t height)
    : m_RendererID(0)
    , m_ColorAttachment(0)
    , m_Width(width)
    , m_Height(height)
{
    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    glGenTextures(1, &m_ColorAttachment);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
    if (m_ColorAttachment)
        glDeleteTextures(1, &m_ColorAttachment);
    if (m_RendererID)
        glDeleteFramebuffers(1, &m_RendererID);
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : m_RendererID(other.m_RendererID)
    , m_ColorAttachment(other.m_ColorAttachment)
    , m_Width(other.m_Width)
    , m_Height(other.m_Height)
{
    other.m_RendererID = 0;
    other.m_ColorAttachment = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
    if (this != &other)
    {
        if (m_ColorAttachment)
            glDeleteTextures(1, &m_ColorAttachment);
        if (m_RendererID)
            glDeleteFramebuffers(1, &m_RendererID);

        m_RendererID = other.m_RendererID;
        m_ColorAttachment = other.m_ColorAttachment;
        m_Width = other.m_Width;
        m_Height = other.m_Height;

        other.m_RendererID = 0;
        other.m_ColorAttachment = 0;
    }
    return *this;
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glViewport(0, 0, m_Width, m_Height);
}

void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(uint32_t width, uint32_t height)
{
    if (width == m_Width && height == m_Height)
        return;

    m_Width = width;
    m_Height = height;

    glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

void Framebuffer::BindColorAttachment(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
}
