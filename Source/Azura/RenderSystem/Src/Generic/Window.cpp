#include "Generic/Window.h"

namespace Azura {
Window::Window(void* windowResource, const U32 width, const U32 height)
  : m_width(width),
    m_height(height),
    m_windowResource(windowResource) {
}

U32 Window::Width() const {
  return m_width;
}

U32 Window::Height() const {
  return m_height;
}

const void* Window::Handle() const {
  return m_windowResource;
}
} // namespace Azura
