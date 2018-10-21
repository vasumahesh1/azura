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

    if (currentTime - previousTime >= 1.0) {
      String windowTitle = String(GetTitle()) + " - FPS: " + std::to_string(frameCount) + " - Frame Time: " + std::
                           to_string(1000.0 / frameCount) + "ms";
      glfwSetWindowTitle(p_window, windowTitle.c_str());

      frameCount   = 0;
      previousTime = currentTime;
    }

    double currCursorX;
    double currCursorY;
    glfwGetCursorPos(p_window, &currCursorX, &currCursorY);

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

    CallUpdateFunction();
    glfwPollEvents();
  }
}

GLFWwindow* GLFWWindow::GetGLFWHandle() const {
  return p_window;
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
