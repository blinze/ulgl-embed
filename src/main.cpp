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

        Vertex triangleVertices[] =
        {
            {{  0.0f,  0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }}, // Top
            {{  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }}, // Bottom-right
            {{ -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }}, // Bottom-left
        };

        uint32_t triangleIndices[] = { 0, 1, 2 };

        VertexArray triangleVAO;
        VertexBuffer triangleVBO(triangleVertices, sizeof(triangleVertices));
        IndexBuffer triangleIBO(triangleIndices, sizeof(triangleIndices) / sizeof(uint32_t));

        triangleVAO.Bind();
        triangleVAO.AddVertexBuffer(triangleVBO);
        triangleVAO.SetIndexBuffer(triangleIBO);

        Vertex quadVertices[] =
        {
            {{ -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }}, // Bottom-left
            {{  1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }}, // Bottom-right
            {{  1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }}, // Top-right
            {{ -1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }}  // Top-left
        };

        uint32_t quadIndices[] = { 0, 1, 2, 2, 3, 0 };

        VertexArray quadVAO;
        VertexBuffer quadVBO(quadVertices, sizeof(quadVertices));
        IndexBuffer quadIBO(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));

        quadVAO.Bind();
        quadVAO.AddVertexBuffer(quadVBO);
        quadVAO.SetIndexBuffer(quadIBO);

        std::cout << "Creating triangle shader..." << std::endl;
        Shader triangleShader("Assets/Shader.vert", "Assets/Shader.frag");
        if (triangleShader.GetID() == 0)
        {
            std::cerr << "Failed to create triangle shader!" << std::endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            PauseConsole();
            return -1;
        }
        CheckGLError("After triangle shader creation");

        std::cout << "Creating texture shader..." << std::endl;
        Shader textureShader("Assets/TextureShader.vert", "Assets/TextureShader.frag");
        if (textureShader.GetID() == 0)
        {
            std::cerr << "Failed to create texture shader!" << std::endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            PauseConsole();
            return -1;
        }
        CheckGLError("After texture shader creation");

        uint32_t halfWidth = WINDOW_WIDTH / 2;

        std::cout << "Creating texture for Ultralight (resolution: " << halfWidth << "x" << WINDOW_HEIGHT << ")..." << std::endl;
        Texture ultralightTexture(halfWidth, WINDOW_HEIGHT, nullptr, GL_RGBA8, GL_BGRA);
        CheckGLError("After texture creation");

        std::cout << "Initializing Ultralight (resolution: " << halfWidth << "x" << WINDOW_HEIGHT << ")..." << std::endl;
        UltralightRenderer ultralight;
        InputEventHandler inputHandler;
        
        if (!ultralight.Initialize(halfWidth, WINDOW_HEIGHT))
        {
            std::cerr << "Failed to initialize Ultralight! Continuing without it..." << std::endl;
        }
        else
        {
            std::cout << "Ultralight initialized successfully!" << std::endl;
            inputHandler.Initialize(window, &ultralight);
            inputHandler.SetViewportRegion(halfWidth, 0, halfWidth, WINDOW_HEIGHT);
            std::cout << "Input event handler initialized for viewport region: (" 
                      << halfWidth << ", 0) - " << halfWidth << "x" << WINDOW_HEIGHT << std::endl;
            
            std::filesystem::path htmlPath = std::filesystem::current_path() / "app" / "build" / "index.html";
            std::cout << "Checking for HTML file at: " << htmlPath << std::endl;
            if (std::filesystem::exists(htmlPath))
            {
                std::cout << "HTML file found!" << std::endl;
            }
            else
            {
                std::cerr << "WARNING: HTML file not found at: " << htmlPath << std::endl;
                std::cerr << "Current working directory: " << std::filesystem::current_path() << std::endl;
                std::filesystem::path fallbackPath = std::filesystem::current_path() / "app" / "index.html";
                if (std::filesystem::exists(fallbackPath))
                {
                    std::cout << "Found fallback HTML file at: " << fallbackPath << std::endl;
                }
            }

            std::cout << "Loading React app..." << std::endl;
            ultralight.LoadURL("file:///app/build/index.html");
            std::cout << "React app load requested." << std::endl;

            if (auto* view = ultralight.GetView())
            {
                view->Focus();
                std::cout << "View focused." << std::endl;
            }
        }

        std::cout << "Entering main loop..." << std::endl;

        while (!glfwWindowShouldClose(window))
        {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            CheckGLError("After clear");

            if (ultralight.IsInitialized())
            {
                ultralight.Update();
                ultralight.Render();

                static bool firstRender = true;
                if (ultralight.IsDirty() || firstRender)
                {
                    auto* bitmap = ultralight.GetBitmap();
                    if (bitmap)
                    {
                        void* pixels = bitmap->LockPixels();
                        if (pixels)
                        {
                            uint32_t width = bitmap->width();
                            uint32_t height = bitmap->height();

                            static int frameCount = 0;
                            if (firstRender || frameCount % 60 == 0)
                            {
                                uint32_t* pixelData = static_cast<uint32_t*>(pixels);
                                uint32_t samplePixel = pixelData[width * height / 2];
                                std::cout << "Frame " << frameCount << ": Bitmap size=" << width << "x" << height
                                          << ", sample pixel=0x" << std::hex << samplePixel << std::dec << std::endl;
                            }
                            frameCount++;

                            ultralightTexture.UpdateData(pixels, width, height);
                            CheckGLError("After texture update");

                            bitmap->UnlockPixels();
                            ultralight.ClearDirty();
                            firstRender = false;
                        }
                        else
                        {
                            std::cerr << "Failed to lock bitmap pixels!" << std::endl;
                        }
                    }
                    else
                    {
                        if (firstRender)
                            std::cerr << "Bitmap is null!" << std::endl;
                    }
                }
            }

            glViewport(0, 0, halfWidth, WINDOW_HEIGHT);
            glDisable(GL_DEPTH_TEST);
            triangleShader.Bind();
            triangleVAO.Bind();
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
            CheckGLError("After triangle draw");

            if (ultralight.IsInitialized())
            {
                glViewport(halfWidth, 0, halfWidth, WINDOW_HEIGHT);
                textureShader.Bind();
                ultralightTexture.Bind(0);

                int textureLocation = glGetUniformLocation(textureShader.GetID(), "u_Texture");
                if (textureLocation != -1)
                {
                    glUniform1i(textureLocation, 0);
                }
                CheckGLError("After shader bind");

                quadVAO.Bind();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
                CheckGLError("After texture draw");
            }

            glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
            glEnable(GL_DEPTH_TEST);

            glfwPollEvents();
            glfwSwapBuffers(window);
        }

        std::cout << "Shutting down..." << std::endl;
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
