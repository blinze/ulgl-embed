#include "Shader.h"
#include <iostream>
#include <fstream>
#include <iterator>

uint32_t Shader::CompileShader(uint32_t type, const std::string& source)
{
    uint32_t id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::string message(length, '\0');
        glGetShaderInfoLog(id, length, &length, message.data());

        std::cerr << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
                  << " Shader compilation failed:\n" << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

uint32_t Shader::CreateShaderProgram(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    uint32_t program = glCreateProgram();

    uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    if (!vs || !fs)
        return 0;

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::string message(length, '\0');
        glGetProgramInfoLog(program, length, &length, message.data());

        std::cerr << "Program link failed:\n" << message << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

std::string Shader::ReadFile(const std::string& path)
{
    std::ifstream stream(path);
    return std::string(std::istreambuf_iterator<char>(stream),
                       std::istreambuf_iterator<char>());
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertexShaderSource = ReadFile(vertexPath);
    std::string fragmentShaderSource = ReadFile(fragmentPath);
    m_RendererID = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

Shader::Shader(Shader&& other) noexcept
    : m_RendererID(other.m_RendererID)
{
    other.m_RendererID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        glDeleteProgram(m_RendererID);
        m_RendererID = other.m_RendererID;
        other.m_RendererID = 0;
    }
    return *this;
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}
