#pragma once
#include "Generic/GLFWWindow.h"
#include <windows.h>

namespace Azura {
class Win32GLFWWindow final : public GLFWWindow {
 public:
  Win32GLFWWindow(const String& title, U32 width, U32 height);
  const void* GetHandle() const override;

  bool Initialize() override;
private:
  HWND m_win32Window;
};
}  // namespace Azura
