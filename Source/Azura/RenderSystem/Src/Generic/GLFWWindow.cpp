// Don't include GLFW's non-sense. We include what we need.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Generic/GLFWWindow.h"

namespace Azura {
namespace {
float GetUpdateRateDelta(UpdateRate rate) {
  switch (rate) {
    case UpdateRate::RateUnlocked:
      return 0.0f;
    case UpdateRate::Rate240:
      return 1.0f / 240.0f;
    case UpdateRate::Rate120:
      return 1.0f / 120.0f;
    case UpdateRate::Rate60:
      return 1.0f / 60.0f;
    case UpdateRate::Rate30:
      return 1.0f / 30.0f;

    default:
      return 0.0f;
  }
}

KeyboardKey GetKey(int key) {
  switch (key) {
    case GLFW_KEY_W:
      return KeyboardKey::W;

    case GLFW_KEY_A:
      return KeyboardKey::A;

    case GLFW_KEY_S:
      return KeyboardKey::S;

    case GLFW_KEY_D:
      return KeyboardKey::D;

    case GLFW_KEY_R:
      return KeyboardKey::R;

    case GLFW_KEY_T:
      return KeyboardKey::T;

    case GLFW_KEY_Q:
      return KeyboardKey::Q;

    case GLFW_KEY_E:
      return KeyboardKey::E;

    case GLFW_KEY_Y:
      return KeyboardKey::Y;

    case GLFW_KEY_ESCAPE:
      return KeyboardKey::Esc;

    case GLFW_KEY_UP:
      return KeyboardKey::Up;

    case GLFW_KEY_DOWN:
      return KeyboardKey::Down;

    case GLFW_KEY_RIGHT:
      return KeyboardKey::Right;

    case GLFW_KEY_LEFT:
      return KeyboardKey::Left;

    default:
      return KeyboardKey::Unmapped;

  }
}
} // namespace


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

  glfwSetKeyCallback(p_window, KeyPressCallback);

  glfwSetWindowUserPointer(p_window, this);

  return true;
}

void GLFWWindow::Destroy() {
  glfwDestroyWindow(p_window);
  glfwTerminate();
}

void GLFWWindow::StartListening() {

  int frameCount{0};
  double previousStatsTime = glfwGetTime();
  double previousFrameTime = glfwGetTime();

  const float rateDelta = GetUpdateRateDelta(m_rate);

  // TODO(vasumahesh1):[GAME]: Need a Performance Timer here

  while (glfwWindowShouldClose(p_window) == GLFW_FALSE) {

    const double currentTime = glfwGetTime();

    double currCursorX;
    double currCursorY;
    glfwGetCursorPos(p_window, &currCursorX, &currCursorY);

    MouseEvent mouseEvent     = {};
    mouseEvent.m_internalType = MouseEventType::MouseUpdate;
    mouseEvent.m_eventX       = float(currCursorX);
    mouseEvent.m_eventY       = float(currCursorY);
    CallMouseEventFunction(mouseEvent);

    if (m_mouseLeftDown) {
      const double diffX = currCursorX - m_prevCursorX;
      const double diffY = currCursorY - m_prevCursorY;

      if (diffX > 0.001 || diffY > 0.001) {
        MouseEvent evt     = {};
        evt.m_internalType = MouseEventType::Drag;
        evt.m_eventX       = float(diffX);
        evt.m_eventY       = float(diffY);
        CallMouseEventFunction(evt);
      }

      m_prevCursorX = currCursorX;
      m_prevCursorY = currCursorY;
    }

    const auto timeDelta  = float(currentTime - previousFrameTime);
    const bool shouldTick = m_rate == UpdateRate::RateUnlocked || (timeDelta > rateDelta);

    if (shouldTick) {
      CallUpdateFunction(timeDelta);

      frameCount++;
      previousFrameTime = currentTime;
    }

    const double statsTimeDelta = currentTime - previousStatsTime;

    if (statsTimeDelta >= 1.0) {
      String windowTitle = String(GetTitle()) + " - FPS: " + std::to_string(frameCount) + " - Frame Time: " + std::
        to_string(1000.0 / frameCount) + "ms";
      glfwSetWindowTitle(p_window, windowTitle.c_str());

      frameCount        = 0;
      previousStatsTime = currentTime;
    }

    glfwPollEvents();
  }
}

void GLFWWindow::ResetCursor() {
  glfwSetCursorPos(p_window, m_midWidth, m_midHeight);
}

GLFWwindow* GLFWWindow::GetGLFWHandle() const {
  return p_window;
}

void GLFWWindow::KeyPressCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
  UNUSED(mods);
  UNUSED(scanCode);

  GLFWWindow* wrapper = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window)); // NOLINT

  if (action == GLFW_PRESS) {
    wrapper->CallKeyEventFunction(KeyEvent(key, GetKey(key), KeyEventType::KeyPress));
  } else if (action == GLFW_RELEASE) {
    wrapper->CallKeyEventFunction(KeyEvent(key, GetKey(key), KeyEventType::KeyRelease));
  }
}

void GLFWWindow::SetCursorState(CursorState state) {
  switch (state) {
    case CursorState::Visible:
      glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      break;
    case CursorState::Hidden:
      glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
      break;
    case CursorState::Disabled:
      glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      break;

    default:
      break;
  }
}

void GLFWWindow::MouseEventCallback(GLFWwindow* window, int button, int action, int mods) {
  UNUSED(mods);

  GLFWWindow* wrapper = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window)); // NOLINT

  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (GLFW_PRESS == action) {
      wrapper->m_mouseLeftDown = true;
      glfwGetCursorPos(window, &(wrapper->m_prevCursorX), &(wrapper->m_prevCursorY));
    } else if (GLFW_RELEASE == action) {
      wrapper->m_mouseLeftDown = false;
    }
  }
}

} // namespace Azura
