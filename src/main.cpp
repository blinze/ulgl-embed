#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

// Uncomment to use Ultralight (requires SDK setup - see ULTRALIGHT_SETUP.md)
// #include "UltralightRenderer.h"

static constexpr uint32_t WINDOW_WIDTH = 1280;
static constexpr uint32_t WINDOW_HEIGHT = 720;

int main(void)
{
    glfwInit();
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                          "ULGL-Embed", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    Vertex vertices[] =
    {
        {{  0.0f,  0.5f }, { 1.0f, 0.0f, 0.0f }},
        {{  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }},
        {{ -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }},
    };

    uint32_t indices[] = { 0, 1, 2 };

    VertexArray vertexArray;
    VertexBuffer vertexBuffer(vertices, sizeof(vertices));
    IndexBuffer indexBuffer(indices, sizeof(indices) / sizeof(uint32_t));

    vertexArray.Bind();
    vertexArray.AddVertexBuffer(vertexBuffer);
    vertexArray.SetIndexBuffer(indexBuffer);

    Shader shader("Assets/Shader.vert", "Assets/Shader.frag");
    shader.Bind();

    // Ultralight integration example (uncomment when SDK is set up):
    /*
    UltralightRenderer ultralight;
    if (ultralight.Initialize(WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        ultralight.LoadHTML("<html><body style='background: #1a1a1a; color: white; font-family: Arial;'><h1>Hello from Ultralight!</h1><p>This is HTML rendered in your OpenGL app!</p></body></html>");
    }
    */

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update and render Ultralight (if initialized)
        /*
        if (ultralight.IsInitialized())
        {
            ultralight.Update();
            ultralight.Render();
            
            // Get bitmap and render to texture if dirty
            if (ultralight.IsDirty())
            {
                auto* bitmap = ultralight.GetBitmap();
                if (bitmap)
                {
                    void* pixels = bitmap->LockPixels();
                    uint32_t width = bitmap->width();
                    uint32_t height = bitmap->height();
                    // TODO: Upload pixels to OpenGL texture here
                    bitmap->UnlockPixels();
                    ultralight.ClearDirty();
                }
            }
        }
        */

        vertexArray.Bind();
        glDrawElements(GL_TRIANGLES, indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
}
