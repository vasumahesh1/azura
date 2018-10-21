// Don't include GLFW's non-sense. We include what we need.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Generic/GLFWWindow.h"

namespace Azura {

GLFWWindow::GLFWWindow(String title, U32 width, U32 height)
  : Window(title, width, height),
    p_window(nullptr) {
}

bool GLFWWindow::Initialize() {
  if (glfwInit() == GLFW_FALSE) {
    return false;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  p_window = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle(), nullptr, nullptr);

  glfwSetMouseButtonCallback(p_window, MouseEventCallback);

  glfwSetKeyCallback(p_window, KeyPressCallback);

  glfwSetWindowUserPointer(p_window, this);

  return true;
}

void GLFWWindow::Destroy() {
  glfwDestroyWindow(p_window);
  glfwTerminate();
}

void GLFWWindow::StartListening() {

  int frameCount{0};
  double previousTime = glfwGetTime();

  // TODO(vasumahesh1):[GAME]: Need a Performance Timer here

  while (glfwWindowShouldClose(p_window) == GLFW_FALSE) {

    const double currentTime = glfwGetTime();
    frameCount++;

    const double timeDelta = currentTime - previousTime;

    if (timeDelta >= 1.0) {
      String windowTitle = String(GetTitle()) + " - FPS: " + std::to_string(frameCount) + " - Frame Time: " + std::
                           to_string(1000.0 / frameCount) + "ms";
      glfwSetWindowTitle(p_window, windowTitle.c_str());

      frameCount   = 0;
      previousTime = currentTime;
    }

    double currCursorX;
    double currCursorY;
    glfwGetCursorPos(p_window, &currCursorX, &currCursorY);

    MouseEvent mouseEvent = {};
    mouseEvent.m_internalType = MouseEventType::MouseUpdate;
    mouseEvent.m_eventX = float(currCursorX);
    mouseEvent.m_eventY = float(currCursorY);
    CallMouseEventFunction(mouseEvent);

    if (m_mouseLeftDown)
    {
      const double diffX = currCursorX - m_prevCursorX;
      const double diffY = currCursorY - m_prevCursorY;

      if (diffX > 0.001 || diffY > 0.001)
      {
        MouseEvent evt = {};
        evt.m_internalType = MouseEventType::Drag;
        evt.m_eventX = float(diffX);
        evt.m_eventY = float(diffY);
        CallMouseEventFunction(evt);
      }

      m_prevCursorX = currCursorX;
      m_prevCursorY = currCursorY;
    }

    if (m_keyPress)
    {
      KeyEventType keyType = KeyEventType::Unmapped;

      switch(m_activeKey)
      {
      case GLFW_KEY_W:
        keyType = KeyEventType::W;
        break;

      case GLFW_KEY_A:
        keyType = KeyEventType::A;
        break;

      case GLFW_KEY_S:
        keyType = KeyEventType::S;
        break;

      case GLFW_KEY_D:
        keyType = KeyEventType::D;
        break;
        
      case GLFW_KEY_ESCAPE:
        keyType = KeyEventType::Esc;
        break;
      }

      CallKeyEventFunction(KeyEvent(m_activeKey, keyType));
    }

    CallUpdateFunction(timeDelta);
    glfwPollEvents();
  }
}

void GLFWWindow::ResetCursor() {
  glfwSetCursorPos(p_window, m_midWidth, m_midHeight);
}

GLFWwindow* GLFWWindow::GetGLFWHandle() const {
  return p_window;
}

void GLFWWindow::KeyPressCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
  UNUSED(mods);
  UNUSED(scanCode);

  GLFWWindow* wrapper = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window)); // NOLINT

  if (action == GLFW_PRESS)
  {
    wrapper->m_keyPress = true;
    wrapper->m_activeKey = key;
  }
  else if (action == GLFW_RELEASE)
  {
    wrapper->m_keyPress = false;
    wrapper->m_activeKey = -1;
  }
}

void GLFWWindow::SetCursorState(CursorState state) {
  switch(state)
  {
    case CursorState::Visible:
      glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
    case CursorState::Hidden:
      glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    break;
    case CursorState::Disabled:
      glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
    
    default:
    break;
  }
}

void GLFWWindow::MouseEventCallback(GLFWwindow* window, int button, int action, int mods) {
  UNUSED(mods);

  GLFWWindow* wrapper = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window)); // NOLINT

  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (GLFW_PRESS == action) {
      wrapper->m_mouseLeftDown = true;
      glfwGetCursorPos(window, &(wrapper->m_prevCursorX), &(wrapper->m_prevCursorY));
    }
    else if (GLFW_RELEASE == action) {
      wrapper->m_mouseLeftDown = false;
    }
  }
}

} // namespace Azura
