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

protected:
  GLFWwindow* GetGLFWHandle() const;

private:
  GLFWwindow* p_window;
};

} // namespace Azura
