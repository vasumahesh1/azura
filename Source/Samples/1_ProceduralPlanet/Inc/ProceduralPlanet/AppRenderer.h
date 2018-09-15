#pragma once
#include <memory>
#include "Generic/Renderer.h"
#include "Generic/Window.h"
#include "Memory/RangeAllocator.h"
#include "Memory/HeapMemoryBuffer.h"

#include "Log/Log.h"
#include "Math/Core.h"

namespace Azura {
struct UniformBufferData
{
  Matrix4f m_model;
  Matrix4f m_view;
  Matrix4f m_proj;
};

class AppRenderer {
public:
  AppRenderer();

  void Initialize();
  void WindowUpdate();
  void Run() const;
  void Destroy() const;

private:
  Memory::HeapMemoryBuffer m_mainBuffer;

  Memory::RangeAllocator m_mainAllocator;
  Memory::RangeAllocator m_drawableAllocator;

  std::unique_ptr<Renderer> m_renderer{nullptr};
  std::unique_ptr<Window> m_window{nullptr};

  Log log_AppRenderer;
};
} // namespace
