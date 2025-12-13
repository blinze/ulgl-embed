#pragma once

#include <glad/glad.h>
#include <cstddef>
#include <cstdint>

class IndexBuffer
{
private:
    uint32_t m_RendererID;
    uint32_t m_Count;
public:
    IndexBuffer(const uint32_t* data, uint32_t count);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    IndexBuffer(IndexBuffer&& other) noexcept;
    IndexBuffer& operator=(IndexBuffer&& other) noexcept;

    void Bind() const;
    void Unbind() const;

    uint32_t GetCount() const { return m_Count; }
    uint32_t GetID() const { return m_RendererID; }
};
