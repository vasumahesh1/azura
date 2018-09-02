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
  return true;
}

void GLFWWindow::Destroy() {
  glfwDestroyWindow(p_window);
  glfwTerminate();
}

void GLFWWindow::StartListening() {
  while (glfwWindowShouldClose(p_window) == GLFW_FALSE) {
    CallUpdateFunction();
    glfwPollEvents();
  }
}

GLFWwindow* GLFWWindow::GetGLFWHandle() const {
  return p_window;
}

} // namespace Azura
