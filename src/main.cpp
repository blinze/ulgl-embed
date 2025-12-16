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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Vertex.h"
#include "Primitives.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Component.h"
#include "UltralightRenderer.h"
#include "InputEvent.h"
#include "JSBridge.h"

static constexpr uint32_t INITIAL_WINDOW_WIDTH = 1280;
static constexpr uint32_t INITIAL_WINDOW_HEIGHT = 720;

static glm::vec3 g_Rotation = glm::vec3(0.0f);
static PrimitiveType g_PrimitiveType = PrimitiveType::Cube;
static bool g_PrimitiveChanged = false;

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

        Component primitiveComponent(windowWidth, windowHeight);
        primitiveComponent.SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        primitiveComponent.EnableDepthTest(true);
        primitiveComponent.SetShader("assets/Shader.vert", "assets/Shader.frag");
        
        Mesh currentMesh = Primitives::CreatePrimitive(g_PrimitiveType);
        primitiveComponent.SetGeometry(currentMesh.vertices.data(), 
                                       static_cast<uint32_t>(currentMesh.vertices.size()),
                                       currentMesh.indices.data(), 
                                       static_cast<uint32_t>(currentMesh.indices.size()));

        Mesh quadMesh = Primitives::CreateQuad();
        VertexArray screenQuadVAO;
        VertexBuffer screenQuadVBO(quadMesh.vertices.data(), quadMesh.vertices.size() * sizeof(Vertex));
        IndexBuffer screenQuadIBO(quadMesh.indices.data(), static_cast<uint32_t>(quadMesh.indices.size()));
        screenQuadVAO.Bind();
        screenQuadVAO.AddVertexBuffer(screenQuadVBO);
        screenQuadVAO.SetIndexBuffer(screenQuadIBO);

        Mesh flippedQuadMesh = Primitives::CreateQuadFlippedUV();
        VertexArray flippedQuadVAO;
        VertexBuffer flippedQuadVBO(flippedQuadMesh.vertices.data(), flippedQuadMesh.vertices.size() * sizeof(Vertex));
        IndexBuffer flippedQuadIBO(flippedQuadMesh.indices.data(), static_cast<uint32_t>(flippedQuadMesh.indices.size()));
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

        if (auto* bridge = ultralight.GetJSBridge())
        {
            bridge->Register("setRotation", [](const JSArgs& args) -> JSValue {
                auto x = JSBridge::GetArg<float>(args, 0);
                auto y = JSBridge::GetArg<float>(args, 1);
                auto z = JSBridge::GetArg<float>(args, 2);
                
                if (x && y && z)
                    g_Rotation = glm::vec3(*x, *y, *z);
                
                return nullptr;
            });
            
            bridge->Register("setPrimitive", [](const JSArgs& args) -> JSValue {
                auto type = JSBridge::GetArg<std::string>(args, 0);
                
                if (type)
                {
                    PrimitiveType newType = StringToPrimitiveType(*type);
                    if (newType != g_PrimitiveType)
                    {
                        g_PrimitiveType = newType;
                        g_PrimitiveChanged = true;
                    }
                }
                
                return nullptr;
            });
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

        uint32_t prevCubeWidth = 0;
        uint32_t prevCubeHeight = 0;
        constexpr float RESOLUTION_SCALE = 2.0f;

        while (!glfwWindowShouldClose(window))
        {
            const ComponentSlot* cubeSlot = ultralight.GetComponentSlot("cube");
            if (cubeSlot && cubeSlot->visible)
            {
                uint32_t targetWidth = static_cast<uint32_t>(cubeSlot->width * RESOLUTION_SCALE);
                uint32_t targetHeight = static_cast<uint32_t>(cubeSlot->height * RESOLUTION_SCALE);
                
                if (targetWidth < 1)
                    targetWidth = 1;
                if (targetHeight < 1)
                    targetHeight = 1;
                
                if (targetWidth != prevCubeWidth || targetHeight != prevCubeHeight)
                {
                    primitiveComponent.Resize(targetWidth, targetHeight);
                    prevCubeWidth = targetWidth;
                    prevCubeHeight = targetHeight;
                }
            }
            else if (prevCubeWidth > 0 || prevCubeHeight > 0)
            {
                prevCubeWidth = 0;
                prevCubeHeight = 0;
            }

            if (g_PrimitiveChanged)
            {
                currentMesh = Primitives::CreatePrimitive(g_PrimitiveType);
                primitiveComponent.SetGeometry(currentMesh.vertices.data(),
                                               static_cast<uint32_t>(currentMesh.vertices.size()),
                                               currentMesh.indices.data(),
                                               static_cast<uint32_t>(currentMesh.indices.size()));
                g_PrimitiveChanged = false;
            }

            float aspect = static_cast<float>(primitiveComponent.GetWidth()) / static_cast<float>(primitiveComponent.GetHeight());
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
            glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, glm::radians(g_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(g_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(g_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 mvp = projection * view * model;
            primitiveComponent.SetMVP(glm::value_ptr(mvp));
            primitiveComponent.Render();

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
            glDisable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            textureShader.Bind();

            ultralightTexture.Bind(0);
            if (texLoc != -1)
                glUniform1i(texLoc, 0);
            flippedQuadVAO.Bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            if (cubeSlot && cubeSlot->visible)
            {
                int x = static_cast<int>(cubeSlot->x);
                int y = static_cast<int>(currentHeight - cubeSlot->y - cubeSlot->height);
                int w = static_cast<int>(cubeSlot->width);
                int h = static_cast<int>(cubeSlot->height);

                glViewport(x, y, w, h);
                primitiveComponent.BindTexture(0);
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
