#pragma once
#include "Window.h"

namespace Azura {

class GLFWWindow final : public Window {

public:
  GLFWWindow(String title, U32 width, U32 height);
  bool Initialize() override;
  void Destroy() override;

  void StartListening() override;
private:
  GLFWwindow* p_window;
};

} // namespace Azura
