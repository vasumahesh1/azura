#include "ProceduralPlanet/AppRenderer.h"
#include "Generic/RawStorageFormat.h"
#include "Generic/RenderSystem.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"

namespace Azura {
using namespace Containers; // NOLINT

AppRenderer::AppRenderer()
  : m_mainBuffer(4096 * 4),
    m_mainAllocator(m_mainBuffer, 2048),
    m_drawableAllocator(m_mainBuffer, 2048) {
}

void AppRenderer::Initialize() {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 1024);
  m_window = RenderSystem::CreateWindow("ProceduralPlanet", 1280, 720);

  ApplicationInfo appInfo;
  appInfo.m_name    = "Procedural Planet";
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, m_window->GetSwapChainRequirements(),
                                            m_mainAllocator, m_drawableAllocator, *m_window);
  m_renderer->SetDrawablePoolCount(1);

  DrawablePoolCreateInfo poolInfo = {};
  poolInfo.m_byteSize     = 2048;
  poolInfo.m_numDrawables = 1;
  DrawablePool& pool      = m_renderer->CreateDrawablePool(poolInfo);

  Slot vertexDataSlot = {};
  vertexDataSlot.m_binding = 0;
  vertexDataSlot.m_rate    = BufferUsageRate::PerVertex;

  Vector<RawStorageFormat> vertexStride = Vector<RawStorageFormat>(ContainerExtent{2, 2}, allocatorTemporary);
  vertexStride[0]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  vertexStride[1]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  pool.AddBufferBinding(vertexDataSlot, vertexStride);

  // Drawable& drawable = pool.CreateDrawable();
  // drawable.AddVertexData(, vertexDataSlot);
}

void AppRenderer::Run() {
}

void AppRenderer::Destroy() {
}
} // namespace Azura
