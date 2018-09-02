#include <utility>
#include "Generic/Window.h"

namespace Azura {
Window::Window(String title, const U32 width, const U32 height)
    : m_width(width),
      m_height(height),
      m_title(std::move(title)),

      p_windowResource(nullptr) {}

void Window::SetUpdateCallback(std::function<void()> eventUpdate) {
  p_updateFunc = eventUpdate;
}

U32 Window::GetWidth() const {
  return m_width;
}

U32 Window::GetHeight() const {
  return m_height;
}

const void* Window::GetHandle() const {
  return p_windowResource;
}

void Window::SetHandle(void* window) {
  p_windowResource = window;
}

const char* Window::GetTitle() const {
  return m_title.c_str();
}
}  // namespace Azura
