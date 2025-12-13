#pragma once

#include <glad/glad.h>
#include <string>
#include <cstdint>

class Shader
{
private:
    uint32_t m_RendererID;

    static uint32_t CompileShader(uint32_t type, const std::string& source);
    static uint32_t CreateShaderProgram(const std::string& vertexSrc, const std::string& fragmentSrc);
    static std::string ReadFile(const std::string& path);
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void Bind() const;
    void Unbind() const;

    uint32_t GetID() const { return m_RendererID; }
};
