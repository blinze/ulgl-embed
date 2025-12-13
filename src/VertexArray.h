#pragma once

#include <glad/glad.h>
#include <memory>
#include <cstdint>

class VertexBuffer;
class IndexBuffer;

class VertexArray
{
private:
    uint32_t m_RendererID;
    uint32_t m_VertexBufferIndex;
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void Bind() const;
    void Unbind() const;

    void AddVertexBuffer(const VertexBuffer& vertexBuffer);
    void SetIndexBuffer(const IndexBuffer& indexBuffer);

    uint32_t GetID() const { return m_RendererID; }
};
