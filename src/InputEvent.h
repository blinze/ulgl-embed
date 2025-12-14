#pragma once

#include <GLFW/glfw3.h>
#include <Ultralight/Ultralight.h>
#include <Ultralight/KeyEvent.h>
#include <Ultralight/KeyCodes.h>
#include <Ultralight/MouseEvent.h>
#include <Ultralight/ScrollEvent.h>
#include <functional>
#include <cstdint>

class UltralightRenderer;

using ResizeCallback = std::function<void(int width, int height)>;

int GLFWKeyToUltralightKey(int glfwKey);
unsigned int GLFWModsToUltralightMods(int glfwMods);
ultralight::MouseEvent::Button GLFWButtonToUltralightButton(int glfwButton);

class InputEventHandler
{
public:
    InputEventHandler();
    ~InputEventHandler() = default;

    void Initialize(GLFWwindow* window, UltralightRenderer* renderer);
    void SetResizeCallback(ResizeCallback callback) { m_ResizeCallback = callback; }

    void SetViewportRegion(int x, int y, int width, int height);
    bool IsInViewportRegion(double x, double y) const;
    void WindowToViewCoords(double windowX, double windowY, int& viewX, int& viewY) const;

    void OnMouseMove(double x, double y);
    void OnMouseButton(int button, int action, int mods);
    void OnScroll(double xoffset, double yoffset);
    void OnKey(int key, int scancode, int action, int mods);
    void OnChar(unsigned int codepoint);
    void OnFocus(bool focused);
    void OnResize(int width, int height);

private:
    UltralightRenderer* m_Renderer;
    GLFWwindow* m_Window;

    int m_ViewportX;
    int m_ViewportY;
    int m_ViewportWidth;
    int m_ViewportHeight;

    double m_MouseX;
    double m_MouseY;

    bool m_MouseInViewport;
    bool m_IsDragging;

    ResizeCallback m_ResizeCallback;
};

void GLFW_MouseMoveCallback(GLFWwindow* window, double x, double y);
void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void GLFW_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void GLFW_CharCallback(GLFWwindow* window, unsigned int codepoint);
void GLFW_FocusCallback(GLFWwindow* window, int focused);
void GLFW_ResizeCallback(GLFWwindow* window, int width, int height);
