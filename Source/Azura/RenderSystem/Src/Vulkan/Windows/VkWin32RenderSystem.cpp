#include "Generic/RenderSystem.h"
#include "Generic/Windows/Win32GLFWWindow.h"

namespace Azura {
std::unique_ptr<Window> RenderSystem::CreateWindow(String title, U32 width, U32 height) {
  return std::make_unique<Win32GLFWWindow>(title, width, height);
}
}  // namespace Azura