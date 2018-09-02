#pragma once
#include "Types.h"

namespace Azura {
class Window {
public:
  Window(U32 width, U32 height);
  const void* GetHandle() const;

protected:
  U32 GetWidth() const;
  U32 GetHeight() const;

  void SetHandle(void* window);

private:
  U32 m_width;
  U32 m_height;
  void* m_windowResource;
};
} // namespace Azura
