#include "Component.h"
#include <glad/glad.h>
#include <iostream>

Component::Component(uint32_t width, uint32_t height)
    : m_Framebuffer(std::make_unique<Framebuffer>(width, height))
    , m_VAO(nullptr)
    , m_VBO(nullptr)
    , m_IBO(nullptr)
    , m_Shader(nullptr)
    , m_Width(width)
    , m_Height(height)
    , m_IndexCount(0)
    , m_RenderCallback(nullptr)
    , m_ClearColor{0.1f, 0.1f, 0.1f, 1.0f}
    , m_MVP{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}
    , m_DepthTestEnabled(false)
{
}

void Component::SetGeometry(const Vertex* vertices, uint32_t vertexCount,
                            const uint32_t* indices, uint32_t indexCount)
{
    m_VAO = std::make_unique<VertexArray>();
    m_VBO = std::make_unique<VertexBuffer>(vertices, vertexCount * sizeof(Vertex));
    m_IBO = std::make_unique<IndexBuffer>(indices, indexCount);
    m_IndexCount = indexCount;

    m_VAO->Bind();
    m_VAO->AddVertexBuffer(*m_VBO);
    m_VAO->SetIndexBuffer(*m_IBO);
}

void Component::SetShader(const std::string& vertexPath, const std::string& fragmentPath)
{
    m_Shader = std::make_unique<Shader>(vertexPath, fragmentPath);
    if (m_Shader->GetID() == 0)
    {
        std::cerr << "Component: Failed to create shader!" << std::endl;
        m_Shader.reset();
    }
}

void Component::SetClearColor(float r, float g, float b, float a)
{
    m_ClearColor[0] = r;
    m_ClearColor[1] = g;
    m_ClearColor[2] = b;
    m_ClearColor[3] = a;
}

void Component::SetMVP(const float* mvp)
{
    for (int i = 0; i < 16; i++)
        m_MVP[i] = mvp[i];
}

void Component::Render()
{
    m_Framebuffer->Bind();
    
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
    
    if (m_DepthTestEnabled)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    if (m_RenderCallback)
        m_RenderCallback();
    else if (m_VAO && m_VBO && m_IBO && m_Shader && m_IndexCount > 0)
    {
        m_Shader->Bind();
        int mvpLoc = glGetUniformLocation(m_Shader->GetID(), "u_MVP");
        if (mvpLoc != -1)
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, m_MVP);
        
        m_VAO->Bind();
        m_VBO->Bind();
        m_IBO->Bind();
        
        glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
    }

    m_Framebuffer->Unbind();
}

void Component::BindTexture(uint32_t slot) const
{
    m_Framebuffer->BindColorAttachment(slot);
}

void Component::Resize(uint32_t width, uint32_t height)
{
    if (width == m_Width && height == m_Height)
        return;

    m_Width = width;
    m_Height = height;
    m_Framebuffer->Resize(width, height);
}
