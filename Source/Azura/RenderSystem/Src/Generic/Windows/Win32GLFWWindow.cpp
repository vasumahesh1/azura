#include "Generic/Windows/Win32GLFWWindow.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Azura {
Win32GLFWWindow::Win32GLFWWindow(const String& title, U32 width, U32 height) : GLFWWindow(title, width, height) {}

const void* Win32GLFWWindow::GetHandle() const {
  return glfwGetWin32Window(GetGLFWHandle());
}
}  // namespace Azura
