#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <exception>
#include <filesystem>
#include <string>
#include <stdexcept>

#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Component.h"
#include "UltralightRenderer.h"
#include "InputEvent.h"

static constexpr uint32_t WINDOW_WIDTH = 1280;
static constexpr uint32_t WINDOW_HEIGHT = 720;

void CheckGLError(const char* location)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "OpenGL Error at " << location << ": " << error << std::endl;
    }
}

void PauseConsole()
{
    std::cout << "\nPress Enter to exit..." << std::endl;
    std::cin.get();
}

int main(void)
{
    std::cout << "=== Application Starting ===" << std::endl;
    std::cerr << "=== Application Starting (stderr) ===" << std::endl;

#ifdef _WIN32
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    std::cout.flush();
    std::cerr.flush();

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE)
    {
        const char* msg = "=== Direct console write ===\n";
        DWORD written;
        WriteConsoleA(hConsole, msg, strlen(msg), &written, NULL);
    }
#endif
    try
    {
        std::filesystem::path exePath;
#ifdef _WIN32
        char exePathBuf[MAX_PATH];
        GetModuleFileNameA(nullptr, exePathBuf, MAX_PATH);
        exePath = std::filesystem::path(exePathBuf);
#elif defined(__APPLE__)
        char exePathBuf[1024];
        uint32_t size = sizeof(exePathBuf);
        if (_NSGetExecutablePath(exePathBuf, &size) == 0)
        {
            exePath = std::filesystem::canonical(exePathBuf);
        }
        else
        {
            throw std::runtime_error("Failed to get executable path (buffer too small)");
        }
#elif defined(__linux__)
        exePath = std::filesystem::canonical("/proc/self/exe");
#else
        if (std::filesystem::exists("/proc/self/exe"))
        {
            exePath = std::filesystem::canonical("/proc/self/exe");
        }
        else
        {
            exePath = std::filesystem::current_path();
        }
#endif
        std::filesystem::path exeDir = exePath.parent_path();
        std::filesystem::current_path(exeDir);
        std::cout << "Working directory set to: " << exeDir << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Warning: Could not set working directory: " << e.what() << std::endl;
        std::cerr << "Continuing with current working directory: " << std::filesystem::current_path() << std::endl;
    }

    std::cout << "=== ULGL-Embed Starting ===" << std::endl;

    try
    {
        std::cout << "Initializing GLFW..." << std::endl;
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
            PauseConsole();
            return -1;
        }

        std::cout << "Creating window..." << std::endl;
        GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                              "ULGL-Embed", nullptr, nullptr);
        if (!window)
        {
            std::cerr << "Failed to create GLFW window!" << std::endl;
            glfwTerminate();
            PauseConsole();
            return -1;
        }

        std::cout << "Setting OpenGL context..." << std::endl;
        glfwMakeContextCurrent(window);

        std::cout << "Loading OpenGL functions..." << std::endl;
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD!" << std::endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            PauseConsole();
            return -1;
        }

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        CheckGLError("After GLAD init");

        uint32_t halfWidth = WINDOW_WIDTH / 2;

        std::cout << "Creating triangle component..." << std::endl;
        Component triangleComponent(halfWidth, WINDOW_HEIGHT);
        triangleComponent.SetClearColor(0.1f, 0.1f, 0.1f);

        Vertex triangleVertices[] = {
            {{  0.0f,  0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
            {{  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
            {{ -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }},
        };
        uint32_t triangleIndices[] = { 0, 1, 2 };
        triangleComponent.SetGeometry(triangleVertices, 3, triangleIndices, 3);
        triangleComponent.SetShader("assets/Shader.vert", "assets/Shader.frag");

        std::cout << "Creating Ultralight component..." << std::endl;
        Component ultralightComponent(halfWidth, WINDOW_HEIGHT);
        ultralightComponent.SetClearColor(1.0f, 1.0f, 1.0f);

        Vertex quadVertices[] = {
            {{ -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }},
            {{  1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }},
            {{  1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
            {{ -1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }}
        };
        uint32_t quadIndices[] = { 0, 1, 2, 2, 3, 0 };
        ultralightComponent.SetGeometry(quadVertices, 4, quadIndices, 6);
        ultralightComponent.SetShader("assets/TextureShader.vert", "assets/TextureShader.frag");

        UltralightRenderer ultralight;
        InputEventHandler inputHandler;
        Texture ultralightTexture(halfWidth, WINDOW_HEIGHT, nullptr, GL_RGBA8, GL_BGRA);

        if (!ultralight.Initialize(halfWidth, WINDOW_HEIGHT))
        {
            std::cerr << "Failed to initialize Ultralight! Continuing without it..." << std::endl;
        }
        else
        {
            std::cout << "Ultralight initialized successfully!" << std::endl;
            inputHandler.Initialize(window, &ultralight);
            inputHandler.SetViewportRegion(halfWidth, 0, halfWidth, WINDOW_HEIGHT);

            std::filesystem::path htmlPath = std::filesystem::current_path() / "app" / "build" / "index.html";
            if (std::filesystem::exists(htmlPath))
            {
                std::cout << "HTML file found at: " << htmlPath << std::endl;
            }
            else
            {
                std::cerr << "WARNING: HTML file not found at: " << htmlPath << std::endl;
            }

            ultralight.LoadURL("file:///app/build/index.html");

            if (auto* view = ultralight.GetView())
            {
                view->Focus();
            }

            ultralightComponent.SetRenderCallback([&]() {
                ultralightComponent.GetShader()->Bind();
                ultralightTexture.Bind(0);
                int texLoc = glGetUniformLocation(ultralightComponent.GetShader()->GetID(), "u_Texture");
                if (texLoc != -1) glUniform1i(texLoc, 0);
                ultralightComponent.GetVAO()->Bind();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            });
        }

        Vertex screenQuadVertices[] = {
            {{ -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
            {{  1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
            {{  1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }},
            {{ -1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }}
        };
        VertexArray screenQuadVAO;
        VertexBuffer screenQuadVBO(screenQuadVertices, sizeof(screenQuadVertices));
        IndexBuffer screenQuadIBO(quadIndices, 6);
        screenQuadVAO.Bind();
        screenQuadVAO.AddVertexBuffer(screenQuadVBO);
        screenQuadVAO.SetIndexBuffer(screenQuadIBO);

        Shader screenShader("assets/TextureShader.vert", "assets/TextureShader.frag");

        while (!glfwWindowShouldClose(window))
        {
            triangleComponent.Render();

            if (ultralight.IsInitialized())
            {
                ultralight.Update();
                ultralight.Render();

                if (ultralight.IsDirty())
                {
                    auto* bitmap = ultralight.GetBitmap();
                    if (bitmap)
                    {
                        void* pixels = bitmap->LockPixels();
                        if (pixels)
                        {
                            ultralightTexture.UpdateData(pixels, bitmap->width(), bitmap->height());
                            bitmap->UnlockPixels();
                            ultralight.ClearDirty();
                        }
                    }
                }

                ultralightComponent.Render();
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            screenShader.Bind();
            int texLoc = glGetUniformLocation(screenShader.GetID(), "u_Texture");

            glViewport(0, 0, halfWidth, WINDOW_HEIGHT);
            triangleComponent.BindTexture(0);
            if (texLoc != -1) glUniform1i(texLoc, 0);
            screenQuadVAO.Bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            if (ultralight.IsInitialized())
            {
                glViewport(halfWidth, 0, halfWidth, WINDOW_HEIGHT);
                ultralightComponent.BindTexture(0);
                if (texLoc != -1) glUniform1i(texLoc, 0);
                screenQuadVAO.Bind();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            }

            glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

            glfwPollEvents();
            glfwSwapBuffers(window);
        }
        
        glfwDestroyWindow(window);
        glfwTerminate();

        std::cout << "=== Program Exited Successfully ===" << std::endl;
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n!!! EXCEPTION CAUGHT: " << e.what() << " !!!" << std::endl;
        PauseConsole();
        return -1;
    }
    catch (...)
    {
        std::cerr << "\n!!! UNKNOWN EXCEPTION CAUGHT !!!" << std::endl;
        PauseConsole();
        return -1;
    }
}
