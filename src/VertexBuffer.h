#pragma once

#include <glad/glad.h>
#include <cstddef>
#include <cstdint>

class VertexBuffer
{
private:
    uint32_t m_RendererID;
public:
    VertexBuffer(const void* data, size_t size);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    void Bind() const;
    void Unbind() const;

    uint32_t GetID() const { return m_RendererID; }
};
