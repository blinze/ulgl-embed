#pragma once

#include "Framebuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Vertex.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

class Component
{
public:
    using RenderCallback = std::function<void()>;
private:
    std::unique_ptr<Framebuffer> m_Framebuffer;
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<IndexBuffer> m_IBO;
    std::unique_ptr<Shader> m_Shader;

    uint32_t m_Width;
    uint32_t m_Height;
    uint32_t m_IndexCount;
    RenderCallback m_RenderCallback;
    float m_ClearColor[4];
public:
    Component(uint32_t width, uint32_t height);
    ~Component() = default;

    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;

    Component(Component&& other) noexcept = default;
    Component& operator=(Component&& other) noexcept = default;

    void SetGeometry(const Vertex* vertices, uint32_t vertexCount, 
                     const uint32_t* indices, uint32_t indexCount);
    void SetShader(const std::string& vertexPath, const std::string& fragmentPath);
    void SetRenderCallback(RenderCallback callback) { m_RenderCallback = std::move(callback); }
    void SetClearColor(float r, float g, float b, float a = 1.0f);

    void Render();
    void BindTexture(uint32_t slot = 0) const;

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }
    Framebuffer* GetFramebuffer() const { return m_Framebuffer.get(); }
    VertexArray* GetVAO() const { return m_VAO.get(); }
    Shader* GetShader() const { return m_Shader.get(); }

    void Resize(uint32_t width, uint32_t height);
};
