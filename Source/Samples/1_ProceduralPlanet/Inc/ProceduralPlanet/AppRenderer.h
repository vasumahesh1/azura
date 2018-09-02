#pragma once
#include <memory>
#include "Generic/Renderer.h"
#include "Generic/Window.h"
#include "Memory/RangeAllocator.h"
#include "Memory/HeapMemoryBuffer.h"

namespace Azura {
class AppRenderer {
public:
  AppRenderer();

  void Initialize();
  void Run();
  void Destroy();

private:
  Memory::HeapMemoryBuffer m_mainBuffer;

  Memory::RangeAllocator m_mainAllocator;
  Memory::RangeAllocator m_drawableAllocator;

  std::unique_ptr<Renderer> m_renderer;
  std::unique_ptr<Window> m_window;
};
} // namespace
