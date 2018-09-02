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

} // namespace Azura
