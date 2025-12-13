#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Vertex.h"

VertexArray::VertexArray()
    : m_VertexBufferIndex(0)
{
    glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_RendererID);
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : m_RendererID(other.m_RendererID), m_VertexBufferIndex(other.m_VertexBufferIndex)
{
    other.m_RendererID = 0;
    other.m_VertexBufferIndex = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    if (this != &other)
    {
        glDeleteVertexArrays(1, &m_RendererID);
        m_RendererID = other.m_RendererID;
        m_VertexBufferIndex = other.m_VertexBufferIndex;
        other.m_RendererID = 0;
        other.m_VertexBufferIndex = 0;
    }
    return *this;
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::AddVertexBuffer(const VertexBuffer& vertexBuffer)
{
    Bind();
    vertexBuffer.Bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(sizeof(float) * 2));

    m_VertexBufferIndex++;
}

void VertexArray::SetIndexBuffer(const IndexBuffer& indexBuffer)
{
    Bind();
    indexBuffer.Bind();
}
