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
  int frameCount{0};
  double previousTime = glfwGetTime();

  // TODO(vasumahesh1):[GAME]: Need a Performance Timer here

  while (glfwWindowShouldClose(p_window) == GLFW_FALSE) {
#ifdef BUILD_DEBUG
    const double currentTime = glfwGetTime();
    frameCount++;

    if (currentTime - previousTime >= 1.0) {
      String windowTitle = String(GetTitle()) + " - FPS: " + std::to_string(frameCount) + " - Frame Time: " + std::
                           to_string(1000.0 / frameCount) + "ms";
      glfwSetWindowTitle(p_window, windowTitle.c_str());

      frameCount   = 0;
      previousTime = currentTime;
    }
#endif

    CallUpdateFunction();
    glfwPollEvents();
  }
}

GLFWwindow* GLFWWindow::GetGLFWHandle() const {
  return p_window;
}

} // namespace Azura
