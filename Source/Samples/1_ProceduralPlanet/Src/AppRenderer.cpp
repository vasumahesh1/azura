#include "ProceduralPlanet/AppRenderer.h"
#include "Generic/RawStorageFormat.h"
#include "Generic/RenderSystem.h"
#include "Generic/Shader.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Math/Transform.h"

namespace Azura {
using namespace Containers; // NOLINT

struct Vertex {
  float m_pos[4];
  float m_col[4];
};

struct Instance {
  float m_pos[4];
};

AppRenderer::AppRenderer()
  : m_mainBuffer(16384),
    m_mainAllocator(m_mainBuffer, 8192),
    m_drawableAllocator(m_mainBuffer, 8192),
    log_AppRenderer(Log("AppRenderer")) {
}

void AppRenderer::Initialize() {
  LOG_INF(log_AppRenderer, LOG_LEVEL, "Starting Init of AppRenderer");

  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);
  m_window = RenderSystem::CreateApplicationWindow("ProceduralPlanet", 1280, 720);

  m_window->SetUpdateCallback([this]()
  {
    WindowUpdate();
  });

  VERIFY_TRUE(log_AppRenderer, m_window->Initialize(), "Cannot Initialize Window");

  ApplicationInfo appInfo;
  appInfo.m_name    = "Procedural Planet";
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  Slot vertexDataSlot      = {};
  vertexDataSlot.m_binding = 0;
  vertexDataSlot.m_rate    = BufferUsageRate::PerVertex;

  Slot instanceDataSlot      = {};
  instanceDataSlot.m_binding = 1;
  instanceDataSlot.m_rate    = BufferUsageRate::PerInstance;

  Slot uniformSlot      = {};
  uniformSlot.m_binding = 0;

  UniformBufferData uboData = {};
  uboData.m_model           = Matrix4f::Identity();
  uboData.m_view            = Transform::LookAt(Vector3f(0.5f, 0.5f, 0.0f), Vector3f(0.5f, 0.5f, -6.0f),
                                                Vector3f(0.0f, 1.0f, 0.0f));
  uboData.m_proj = Transform::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

  // TODO(vasumahesh1):[Q]:Allocator?
  ApplicationRequirements applicationRequirements = {};
  applicationRequirements.m_clearColor[0]         = 0.2f;
  applicationRequirements.m_clearColor[1]         = 0.2f;
  applicationRequirements.m_clearColor[2]         = 0.2f;

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            m_window->GetSwapChainRequirements(), m_mainAllocator, m_drawableAllocator,
                                            *m_window);
  m_renderer->SetDrawablePoolCount(1);

  auto vertShader = RenderSystem::
    CreateShader(*m_renderer, "./Shaders/" + m_renderer->GetRenderingAPI() + "/Terrain.vs", log_AppRenderer);
  vertShader->SetStage(ShaderStage::Vertex);

  auto pixelShader = RenderSystem::
    CreateShader(*m_renderer, "./Shaders/" + m_renderer->GetRenderingAPI() + "/Terrain.ps", log_AppRenderer);
  pixelShader->SetStage(ShaderStage::Pixel);

  DrawablePoolCreateInfo poolInfo(allocatorTemporary);
  poolInfo.m_byteSize                    = 4096;
  poolInfo.m_numDrawables                = 1;
  poolInfo.m_numShaders                  = 2;
  poolInfo.m_drawType                    = DrawType::InstancedIndexed;
  poolInfo.m_slotInfo.m_numVertexSlots   = 1;
  poolInfo.m_slotInfo.m_numInstanceSlots = 1;

  // UBO Info
  poolInfo.m_uniformBuffers.Reserve(1);
  poolInfo.m_uniformBuffers.PushBack(std::make_pair(uniformSlot,
                                                    UniformBufferDesc{
                                                      sizeof(UniformBufferData), 1, ShaderStage::Vertex
                                                    }));

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  pool.AddShader(*vertShader);
  pool.AddShader(*pixelShader);

  Vector<RawStorageFormat> vertexStride = Vector<RawStorageFormat>(ContainerExtent{2}, allocatorTemporary);
  vertexStride[0]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  vertexStride[1]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  pool.AddBufferBinding(vertexDataSlot, vertexStride);

  Vector<RawStorageFormat> instanceStride = Vector<RawStorageFormat>(ContainerExtent{1}, allocatorTemporary);
  instanceStride[0]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  pool.AddBufferBinding(instanceDataSlot, instanceStride);

  Vector<Vertex> vertexData = Vector<Vertex>({
    Vertex{{0, 0, 1, 1}, {1, 0, 0, 1}},
    Vertex{{1, 0, 1, 1}, {0, 1, 0, 1}},
    Vertex{{1, 1, 1, 1}, {0, 0, 1, 1}},
    Vertex{{0, 1, 1, 1}, {1, 1, 1, 1}}
  }, allocatorTemporary);

  Vector<Instance> instanceData = Vector<Instance>({
    Instance{{-2, 0, 0, 1}},
    Instance{{2, 0, 0, 1}}
  }, allocatorTemporary);

  Vector<U32> indexData = Vector<U32>({
    0, 2, 1,
    2, 0, 3
  }, allocatorTemporary);

  const auto bufferStart      = reinterpret_cast<U8*>(vertexData.Data());   // NOLINT
  const auto instanceStart    = reinterpret_cast<U8*>(instanceData.Data()); // NOLINT
  const auto indexBufferStart = reinterpret_cast<U8*>(indexData.Data());    // NOLINT
  const auto uboDataBuffer    = reinterpret_cast<U8*>(&uboData);            // NOLINT

  // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = vertexData.GetSize();
  createInfo.m_indexCount       = indexData.GetSize();
  createInfo.m_instanceCount    = 2;
  createInfo.m_indexType        = RawStorageFormat::R32_UINT;

  const auto drawableId = pool.CreateDrawable(createInfo);
  pool.BindVertexData(drawableId, vertexDataSlot, bufferStart, vertexData.GetSize() * sizeof(Vertex));
  pool.BindInstanceData(drawableId, instanceDataSlot, instanceStart, instanceData.GetSize() * sizeof(Instance));
  pool.SetIndexData(drawableId, indexBufferStart, indexData.GetSize() * sizeof(U32));
  pool.BindUniformData(drawableId, uniformSlot, uboDataBuffer, sizeof(UniformBufferData));

  // All Drawables Done
  m_renderer->Submit();

  LOG_INF(log_AppRenderer, LOG_LEVEL, "Initialized AppRenderer");
}

void AppRenderer::WindowUpdate() {
  m_renderer->RenderFrame();
}

void AppRenderer::Run() const {
  m_renderer->RenderFrame();
  m_renderer->RenderFrame();
  m_renderer->RenderFrame();
  m_renderer->SnapshotFrame("BasicInstancingTest.data");

  LOG_INF(log_AppRenderer, LOG_LEVEL, "Running AppRenderer");
  m_window->StartListening();
}

void AppRenderer::Destroy() const {
  m_window->Destroy();
}
} // namespace Azura
