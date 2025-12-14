#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdio>
#include <cstring>
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

static constexpr uint32_t INITIAL_WINDOW_WIDTH = 1280;
static constexpr uint32_t INITIAL_WINDOW_HEIGHT = 720;

void CheckGLError(const char* location)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
        std::cerr << "OpenGL Error at " << location << ": " << error << std::endl;
}

int main(void)
{
#ifdef _WIN32
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
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
            exePath = std::filesystem::canonical(exePathBuf);
#elif defined(__linux__)
        exePath = std::filesystem::canonical("/proc/self/exe");
#endif
        if (!exePath.empty())
        {
            std::filesystem::current_path(exePath.parent_path());
            std::cout << "Working directory: " << exePath.parent_path() << std::endl;
        }
    }
    catch (...) {}

    try
    {
        if (!glfwInit())
            return -1;

        GLFWwindow* window = glfwCreateWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "ULGL-Embed", nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            glfwDestroyWindow(window);
            glfwTerminate();
            return -1;
        }

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        Component triangleComponent(windowWidth, windowHeight);
        triangleComponent.SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        Vertex triangleVertices[] =
        {
            {{  0.0f,  0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
            {{  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
            {{ -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }},
        };
        
        uint32_t triangleIndices[] = { 0, 1, 2 };
        triangleComponent.SetGeometry(triangleVertices, 3, triangleIndices, 3);
        triangleComponent.SetShader("assets/Shader.vert", "assets/Shader.frag");

        Vertex quadVertices[] =
        {
            {{ -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
            {{  1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
            {{  1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }},
            {{ -1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }}
        };
        uint32_t quadIndices[] = { 0, 1, 2, 2, 3, 0 };

        VertexArray screenQuadVAO;
        VertexBuffer screenQuadVBO(quadVertices, sizeof(quadVertices));
        IndexBuffer screenQuadIBO(quadIndices, 6);
        screenQuadVAO.Bind();
        screenQuadVAO.AddVertexBuffer(screenQuadVBO);
        screenQuadVAO.SetIndexBuffer(screenQuadIBO);

        Vertex flippedQuadVertices[] = {
            {{ -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }}, // Bottom-left: tex (0, 1)
            {{  1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }}, // Bottom-right: tex (1, 1)
            {{  1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }}, // Top-right: tex (1, 0)
            {{ -1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }}  // Top-left: tex (0, 0)
        };

        VertexArray flippedQuadVAO;
        VertexBuffer flippedQuadVBO(flippedQuadVertices, sizeof(flippedQuadVertices));
        IndexBuffer flippedQuadIBO(quadIndices, 6);
        flippedQuadVAO.Bind();
        flippedQuadVAO.AddVertexBuffer(flippedQuadVBO);
        flippedQuadVAO.SetIndexBuffer(flippedQuadIBO);

        Shader textureShader("assets/TextureShader.vert", "assets/TextureShader.frag");

        UltralightRenderer ultralight;
        InputEventHandler inputHandler;
        Texture ultralightTexture(windowWidth, windowHeight, nullptr, GL_RGBA8, GL_BGRA);

        if (!ultralight.Initialize(windowWidth, windowHeight))
        {
            std::cerr << "Failed to initialize Ultralight!" << std::endl;
            return -1;
        }

        inputHandler.Initialize(window, &ultralight);
        inputHandler.SetViewportRegion(0, 0, windowWidth, windowHeight);
        
        inputHandler.SetResizeCallback([&ultralightTexture](int width, int height)
        {
            ultralightTexture.Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        });

        ultralight.LoadURL("file:///app/build/index.html");

        if (auto* view = ultralight.GetView())
            view->Focus();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        textureShader.Bind();
        int texLoc = glGetUniformLocation(textureShader.GetID(), "u_Texture");

        uint32_t prevTriangleWidth = 0;
        uint32_t prevTriangleHeight = 0;
        constexpr float RESOLUTION_SCALE = 2.0f;

        while (!glfwWindowShouldClose(window))
        {
            const ComponentSlot* triangleSlot = ultralight.GetComponentSlot("triangle");
            if (triangleSlot && triangleSlot->visible)
            {
                uint32_t targetWidth = static_cast<uint32_t>(triangleSlot->width * RESOLUTION_SCALE);
                uint32_t targetHeight = static_cast<uint32_t>(triangleSlot->height * RESOLUTION_SCALE);
                
                if (targetWidth < 1)
                    targetWidth = 1;
                if (targetHeight < 1)
                    targetHeight = 1;
                
                if (targetWidth != prevTriangleWidth || targetHeight != prevTriangleHeight)
                {
                    triangleComponent.Resize(targetWidth, targetHeight);
                    prevTriangleWidth = targetWidth;
                    prevTriangleHeight = targetHeight;
                }
            }
            else if (prevTriangleWidth > 0 || prevTriangleHeight > 0)
            {
                prevTriangleWidth = 0;
                prevTriangleHeight = 0;
            }

            triangleComponent.Render();

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

            int currentWidth, currentHeight;
            glfwGetFramebufferSize(window, &currentWidth, &currentHeight);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, currentWidth, currentHeight);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            textureShader.Bind();

            ultralightTexture.Bind(0);
            if (texLoc != -1)
                glUniform1i(texLoc, 0);
            flippedQuadVAO.Bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            if (triangleSlot && triangleSlot->visible)
            {
                int x = static_cast<int>(triangleSlot->x);
                int y = static_cast<int>(currentHeight - triangleSlot->y - triangleSlot->height);
                int w = static_cast<int>(triangleSlot->width);
                int h = static_cast<int>(triangleSlot->height);

                glViewport(x, y, w, h);
                triangleComponent.BindTexture(0);
                if (texLoc != -1)
                    glUniform1i(texLoc, 0);
                screenQuadVAO.Bind();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            }

            glViewport(0, 0, currentWidth, currentHeight);

            glfwPollEvents();
            glfwSwapBuffers(window);
        }

        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
}
