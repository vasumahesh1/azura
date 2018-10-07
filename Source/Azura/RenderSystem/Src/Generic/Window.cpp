#include "Generic/Window.h"
#include <utility>

namespace Azura {
Window::Window(String title, const U32 width, const U32 height)
    : m_width(width),
      m_height(height),
      m_title(std::move(title)),

      p_windowResource(nullptr) {}

void Window::SetUpdateCallback(std::function<void()> eventUpdate) {
  p_updateFunc = eventUpdate;
}

ViewportDimensions Window::GetViewport() const {
  ViewportDimensions dimension{};
  dimension.m_x        = 0;
  dimension.m_y        = 0;
  dimension.m_width    = static_cast<float>(m_width);
  dimension.m_height   = static_cast<float>(m_height);
  dimension.m_minDepth = 0.0f;
  dimension.m_maxDepth = 1.0f;
  return dimension;
}

SwapChainRequirements Window::GetSwapChainRequirements() const {
  SwapChainRequirements requirement;
  requirement.m_colorSpace = ColorSpace::SRGB;
  requirement.m_format = RawStorageFormat::B8G8R8A8_UNORM;
  requirement.m_depthFormat = RawStorageFormat::D32_FLOAT;
  requirement.m_extent = Bounds2D(m_width, m_height);
  requirement.m_framesInFlight = 2u;

  return requirement;
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
