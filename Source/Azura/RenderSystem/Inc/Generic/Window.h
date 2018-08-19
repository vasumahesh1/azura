#pragma once
#include "Types.h"


namespace Azura {
class Window {
public:
  Window(void* windowResource, U32 width, U32 height);

protected:
  U32 Width() const;
  U32 Height() const;
  const void* Handle() const;

private:
  U32 m_width;
  U32 m_height;
  const void* m_windowResource;
};
} // namespace Azura
