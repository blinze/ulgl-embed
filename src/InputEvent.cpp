#include "InputEvent.h"
#include "UltralightRenderer.h"
#include <algorithm>
#include <iostream>

int GLFWKeyToUltralightKey(int glfwKey)
{
    using namespace ultralight::KeyCodes;
    switch (glfwKey)
    {
        case GLFW_KEY_SPACE:         return GK_SPACE;
        case GLFW_KEY_APOSTROPHE:    return GK_OEM_7;
        case GLFW_KEY_COMMA:         return GK_OEM_COMMA;
        case GLFW_KEY_MINUS:         return GK_OEM_MINUS;
        case GLFW_KEY_PERIOD:        return GK_OEM_PERIOD;
        case GLFW_KEY_SLASH:         return GK_OEM_2;
        case GLFW_KEY_0:             return GK_0;
        case GLFW_KEY_1:             return GK_1;
        case GLFW_KEY_2:             return GK_2;
        case GLFW_KEY_3:             return GK_3;
        case GLFW_KEY_4:             return GK_4;
        case GLFW_KEY_5:             return GK_5;
        case GLFW_KEY_6:             return GK_6;
        case GLFW_KEY_7:             return GK_7;
        case GLFW_KEY_8:             return GK_8;
        case GLFW_KEY_9:             return GK_9;
        case GLFW_KEY_SEMICOLON:     return GK_OEM_1;
        case GLFW_KEY_EQUAL:         return GK_OEM_PLUS;
        case GLFW_KEY_A:             return GK_A;
        case GLFW_KEY_B:             return GK_B;
        case GLFW_KEY_C:             return GK_C;
        case GLFW_KEY_D:             return GK_D;
        case GLFW_KEY_E:             return GK_E;
        case GLFW_KEY_F:             return GK_F;
        case GLFW_KEY_G:             return GK_G;
        case GLFW_KEY_H:             return GK_H;
        case GLFW_KEY_I:             return GK_I;
        case GLFW_KEY_J:             return GK_J;
        case GLFW_KEY_K:             return GK_K;
        case GLFW_KEY_L:             return GK_L;
        case GLFW_KEY_M:             return GK_M;
        case GLFW_KEY_N:             return GK_N;
        case GLFW_KEY_O:             return GK_O;
        case GLFW_KEY_P:             return GK_P;
        case GLFW_KEY_Q:             return GK_Q;
        case GLFW_KEY_R:             return GK_R;
        case GLFW_KEY_S:             return GK_S;
        case GLFW_KEY_T:             return GK_T;
        case GLFW_KEY_U:             return GK_U;
        case GLFW_KEY_V:             return GK_V;
        case GLFW_KEY_W:             return GK_W;
        case GLFW_KEY_X:             return GK_X;
        case GLFW_KEY_Y:             return GK_Y;
        case GLFW_KEY_Z:             return GK_Z;
        case GLFW_KEY_LEFT_BRACKET:  return GK_OEM_4;
        case GLFW_KEY_BACKSLASH:     return GK_OEM_5;
        case GLFW_KEY_RIGHT_BRACKET: return GK_OEM_6;
        case GLFW_KEY_GRAVE_ACCENT:  return GK_OEM_3;
        case GLFW_KEY_ESCAPE:        return GK_ESCAPE;
        case GLFW_KEY_ENTER:         return GK_RETURN;
        case GLFW_KEY_TAB:           return GK_TAB;
        case GLFW_KEY_BACKSPACE:     return GK_BACK;
        case GLFW_KEY_INSERT:        return GK_INSERT;
        case GLFW_KEY_DELETE:        return GK_DELETE;
        case GLFW_KEY_RIGHT:         return GK_RIGHT;
        case GLFW_KEY_LEFT:          return GK_LEFT;
        case GLFW_KEY_DOWN:          return GK_DOWN;
        case GLFW_KEY_UP:            return GK_UP;
        case GLFW_KEY_PAGE_UP:       return GK_PRIOR;
        case GLFW_KEY_PAGE_DOWN:     return GK_NEXT;
        case GLFW_KEY_HOME:          return GK_HOME;
        case GLFW_KEY_END:           return GK_END;
        case GLFW_KEY_CAPS_LOCK:     return GK_CAPITAL;
        case GLFW_KEY_SCROLL_LOCK:   return GK_SCROLL;
        case GLFW_KEY_NUM_LOCK:      return GK_NUMLOCK;
        case GLFW_KEY_PRINT_SCREEN:  return GK_SNAPSHOT;
        case GLFW_KEY_PAUSE:         return GK_PAUSE;
        case GLFW_KEY_F1:            return GK_F1;
        case GLFW_KEY_F2:            return GK_F2;
        case GLFW_KEY_F3:            return GK_F3;
        case GLFW_KEY_F4:            return GK_F4;
        case GLFW_KEY_F5:            return GK_F5;
        case GLFW_KEY_F6:            return GK_F6;
        case GLFW_KEY_F7:            return GK_F7;
        case GLFW_KEY_F8:            return GK_F8;
        case GLFW_KEY_F9:            return GK_F9;
        case GLFW_KEY_F10:           return GK_F10;
        case GLFW_KEY_F11:           return GK_F11;
        case GLFW_KEY_F12:           return GK_F12;
        case GLFW_KEY_KP_0:          return GK_NUMPAD0;
        case GLFW_KEY_KP_1:          return GK_NUMPAD1;
        case GLFW_KEY_KP_2:          return GK_NUMPAD2;
        case GLFW_KEY_KP_3:          return GK_NUMPAD3;
        case GLFW_KEY_KP_4:          return GK_NUMPAD4;
        case GLFW_KEY_KP_5:          return GK_NUMPAD5;
        case GLFW_KEY_KP_6:          return GK_NUMPAD6;
        case GLFW_KEY_KP_7:          return GK_NUMPAD7;
        case GLFW_KEY_KP_8:          return GK_NUMPAD8;
        case GLFW_KEY_KP_9:          return GK_NUMPAD9;
        case GLFW_KEY_KP_DECIMAL:    return GK_DECIMAL;
        case GLFW_KEY_KP_DIVIDE:     return GK_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY:   return GK_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT:   return GK_SUBTRACT;
        case GLFW_KEY_KP_ADD:        return GK_ADD;
        case GLFW_KEY_KP_ENTER:      return GK_RETURN;
        case GLFW_KEY_LEFT_SHIFT:    return GK_SHIFT;
        case GLFW_KEY_LEFT_CONTROL:  return GK_CONTROL;
        case GLFW_KEY_LEFT_ALT:      return GK_MENU;
        case GLFW_KEY_LEFT_SUPER:    return GK_LWIN;
        case GLFW_KEY_RIGHT_SHIFT:   return GK_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL: return GK_CONTROL;
        case GLFW_KEY_RIGHT_ALT:     return GK_MENU;
        case GLFW_KEY_RIGHT_SUPER:   return GK_RWIN;
        case GLFW_KEY_MENU:          return GK_APPS;
        default:                     return GK_UNKNOWN;
    }
}

unsigned int GLFWModsToUltralightMods(int glfwMods)
{
    unsigned int mods = 0;
    if (glfwMods & GLFW_MOD_ALT)
        mods |= ultralight::KeyEvent::kMod_AltKey;
    if (glfwMods & GLFW_MOD_CONTROL)
        mods |= ultralight::KeyEvent::kMod_CtrlKey;
    if (glfwMods & GLFW_MOD_SUPER)
        mods |= ultralight::KeyEvent::kMod_MetaKey;
    if (glfwMods & GLFW_MOD_SHIFT)
        mods |= ultralight::KeyEvent::kMod_ShiftKey;
    return mods;
}

ultralight::MouseEvent::Button GLFWButtonToUltralightButton(int glfwButton)
{
    switch (glfwButton)
    {
        case GLFW_MOUSE_BUTTON_LEFT: return ultralight::MouseEvent::kButton_Left;
        case GLFW_MOUSE_BUTTON_MIDDLE: return ultralight::MouseEvent::kButton_Middle;
        case GLFW_MOUSE_BUTTON_RIGHT: return ultralight::MouseEvent::kButton_Right;
        default: return ultralight::MouseEvent::kButton_None;
    }
}

InputEventHandler::InputEventHandler()
    : m_Renderer(nullptr)
    , m_Window(nullptr)
    , m_ViewportX(0)
    , m_ViewportY(0)
    , m_ViewportWidth(0)
    , m_ViewportHeight(0)
    , m_MouseX(0)
    , m_MouseY(0)
    , m_MouseInViewport(false)
    , m_IsDragging(false)
{
}

void InputEventHandler::Initialize(GLFWwindow* window, UltralightRenderer* renderer)
{
    m_Window = window;
    m_Renderer = renderer;

    glfwSetWindowUserPointer(window, this);

    glfwSetCursorPosCallback(window, GLFW_MouseMoveCallback);
    glfwSetMouseButtonCallback(window, GLFW_MouseButtonCallback);
    glfwSetScrollCallback(window, GLFW_ScrollCallback);
    glfwSetKeyCallback(window, GLFW_KeyCallback);
    glfwSetCharCallback(window, GLFW_CharCallback);
    glfwSetWindowFocusCallback(window, GLFW_FocusCallback);
    glfwSetFramebufferSizeCallback(window, GLFW_ResizeCallback);

    std::cout << "[Input] Event handler initialized" << std::endl;
}

void InputEventHandler::SetViewportRegion(int x, int y, int width, int height)
{
    m_ViewportX = x;
    m_ViewportY = y;
    m_ViewportWidth = width;
    m_ViewportHeight = height;
}

bool InputEventHandler::IsInViewportRegion(double x, double y) const
{
    return x >= m_ViewportX && x < (m_ViewportX + m_ViewportWidth) &&
           y >= m_ViewportY && y < (m_ViewportY + m_ViewportHeight);
}

void InputEventHandler::WindowToViewCoords(double windowX, double windowY, int& viewX, int& viewY) const
{
    viewX = static_cast<int>(windowX - m_ViewportX);
    viewY = static_cast<int>(windowY - m_ViewportY);
}

void InputEventHandler::OnMouseMove(double x, double y)
{
    m_MouseX = x;
    m_MouseY = y;

    if (!m_Renderer || !m_Renderer->IsInitialized())
        return;

    auto* view = m_Renderer->GetView();
    if (!view)
        return;

    m_MouseInViewport = IsInViewportRegion(x, y);

    if (m_MouseInViewport || m_IsDragging)
    {
        int viewX, viewY;
        WindowToViewCoords(x, y, viewX, viewY);

        ultralight::MouseEvent evt;
        evt.type = ultralight::MouseEvent::kType_MouseMoved;
        evt.x = viewX;
        evt.y = viewY;
        evt.button = ultralight::MouseEvent::kButton_None;
        view->FireMouseEvent(evt);
    }
}

void InputEventHandler::OnMouseButton(int button, int action, int mods)
{
    if (!m_Renderer || !m_Renderer->IsInitialized())
        return;

    auto* view = m_Renderer->GetView();
    if (!view)
        return;

    bool inViewport = IsInViewportRegion(m_MouseX, m_MouseY);

    if (action == GLFW_PRESS && !inViewport)
        return;

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
            m_IsDragging = true;
        else if (action == GLFW_RELEASE)
            m_IsDragging = false;
    }

    int viewX, viewY;
    WindowToViewCoords(m_MouseX, m_MouseY, viewX, viewY);

    if (!inViewport)
    {
        viewX = std::max(0, std::min(viewX, m_ViewportWidth - 1));
        viewY = std::max(0, std::min(viewY, m_ViewportHeight - 1));
    }

    ultralight::MouseEvent evt;
    evt.type = (action == GLFW_PRESS) ? ultralight::MouseEvent::kType_MouseDown 
                                       : ultralight::MouseEvent::kType_MouseUp;
    evt.x = viewX;
    evt.y = viewY;
    evt.button = GLFWButtonToUltralightButton(button);
    view->FireMouseEvent(evt);
}

void InputEventHandler::OnScroll(double xoffset, double yoffset)
{
    if (!m_Renderer || !m_Renderer->IsInitialized())
        return;

    auto* view = m_Renderer->GetView();
    if (!view)
        return;

    if (!IsInViewportRegion(m_MouseX, m_MouseY))
        return;

    ultralight::ScrollEvent evt;
    evt.type = ultralight::ScrollEvent::kType_ScrollByPixel;
    evt.delta_x = static_cast<int>(xoffset * 32);
    evt.delta_y = static_cast<int>(yoffset * 32);
    view->FireScrollEvent(evt);
}

void InputEventHandler::OnKey(int key, int scancode, int action, int mods)
{
    if (!m_Renderer || !m_Renderer->IsInitialized())
        return;

    auto* view = m_Renderer->GetView();
    if (!view)
        return;

    if (action == GLFW_REPEAT)
        return;

    ultralight::KeyEvent evt;
    evt.type = (action == GLFW_PRESS) ? ultralight::KeyEvent::kType_RawKeyDown 
                                       : ultralight::KeyEvent::kType_KeyUp;
    evt.virtual_key_code = GLFWKeyToUltralightKey(key);
    evt.native_key_code = scancode;
    evt.modifiers = GLFWModsToUltralightMods(mods);
    
    ultralight::GetKeyIdentifierFromVirtualKeyCode(evt.virtual_key_code, evt.key_identifier);
    
    view->FireKeyEvent(evt);

}

void InputEventHandler::OnChar(unsigned int codepoint)
{
    if (!m_Renderer || !m_Renderer->IsInitialized())
        return;

    auto* view = m_Renderer->GetView();
    if (!view)
        return;

    ultralight::KeyEvent evt;
    evt.type = ultralight::KeyEvent::kType_Char;
    evt.modifiers = 0;
    
    ultralight::String text = ultralight::String32(reinterpret_cast<const char32_t*>(&codepoint), 1);
    evt.text = text;
    evt.unmodified_text = text;
    
    view->FireKeyEvent(evt);
}

void InputEventHandler::OnFocus(bool focused)
{
    if (!m_Renderer || !m_Renderer->IsInitialized())
        return;

    auto* view = m_Renderer->GetView();
    if (!view)
        return;

    if (focused)
        view->Focus();
    else
        view->Unfocus();
}

void InputEventHandler::OnResize(int width, int height)
{
}

void GLFW_MouseMoveCallback(GLFWwindow* window, double x, double y)
{
    auto* handler = static_cast<InputEventHandler*>(glfwGetWindowUserPointer(window));
    if (handler)
        handler->OnMouseMove(x, y);
}

void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto* handler = static_cast<InputEventHandler*>(glfwGetWindowUserPointer(window));
    if (handler)
        handler->OnMouseButton(button, action, mods);
}

void GLFW_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* handler = static_cast<InputEventHandler*>(glfwGetWindowUserPointer(window));
    if (handler)
        handler->OnScroll(xoffset, yoffset);
}

void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto* handler = static_cast<InputEventHandler*>(glfwGetWindowUserPointer(window));
    if (handler)
        handler->OnKey(key, scancode, action, mods);
}

void GLFW_CharCallback(GLFWwindow* window, unsigned int codepoint)
{
    auto* handler = static_cast<InputEventHandler*>(glfwGetWindowUserPointer(window));
    if (handler)
        handler->OnChar(codepoint);
}

void GLFW_FocusCallback(GLFWwindow* window, int focused)
{
    auto* handler = static_cast<InputEventHandler*>(glfwGetWindowUserPointer(window));
    if (handler)
        handler->OnFocus(focused != 0);
}

void GLFW_ResizeCallback(GLFWwindow* window, int width, int height)
{
    auto* handler = static_cast<InputEventHandler*>(glfwGetWindowUserPointer(window));
    if (handler)
        handler->OnResize(width, height);
}
