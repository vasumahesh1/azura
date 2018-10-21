#pragma once
#include "Window.h"

struct GLFWwindow;

namespace Azura {

class GLFWWindow : public Window {
 public:
  GLFWWindow(String title, U32 width, U32 height);
  bool Initialize() override;
  void Destroy() override;

  void StartListening() override;

  void ResetCursor() override;

  void SetCursorState(CursorState state) override;

 protected:
  GLFWwindow* GetGLFWHandle() const;

 private:
  static void MouseEventCallback(GLFWwindow* window, int button, int action, int mods);
  static void KeyPressCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);

  bool m_mouseLeftDown{false};
  bool m_keyPress{false};

  int m_activeKey{-1};

  double m_prevCursorX{0};
  double m_prevCursorY{0};

  GLFWwindow* p_window;
};

}  // namespace Azura
