#include "Generic/Window.h"

namespace Azura {
Window::Window(const U32 width, const U32 height)
	: m_width(width),
		m_height(height),

		m_windowResource(nullptr) {
}

U32 Window::GetWidth() const {
	return m_width;
}

U32 Window::GetHeight() const {
	return m_height;
}

const void* Window::GetHandle() const {
	return m_windowResource;
}

void Window::SetHandle(void* window) {
	m_windowResource = window;
}
} // namespace Azura
