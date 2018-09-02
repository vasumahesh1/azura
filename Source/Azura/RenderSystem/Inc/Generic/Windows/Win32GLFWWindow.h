#pragma once
#include "Generic/GLFWWindow.h"

namespace Azura {
class Win32GLFWWindow final : public GLFWWindow {
 public:
  Win32GLFWWindow(const String& title, U32 width, U32 height);
  const void* GetHandle() const override;
};
}  // namespace Azura
