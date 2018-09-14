#include "ProceduralPlanet/AppRenderer.h"
#include "Generic/RawStorageFormat.h"
#include "Generic/RenderSystem.h"
#include "Generic/Shader.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"

namespace Azura {
using namespace Containers; // NOLINT

struct Vertex {
  float m_pos[4];
  float m_col[4];
};

AppRenderer::AppRenderer()
  : m_mainBuffer(4096 * 4),
    m_mainAllocator(m_mainBuffer, 2048),
    m_drawableAllocator(m_mainBuffer, 2048),
    log_AppRenderer(Log("AppRenderer")) {
}

void AppRenderer::Initialize() {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 1024);
  m_window = RenderSystem::CreateApplicationWindow("ProceduralPlanet", 1280, 720);

  m_window->SetUpdateCallback([this] ()
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

  const int uboBinding = 0;

  UniformBufferData uboData = {};
  uboData.m_model = mathfu::Matrix<float, 4, 4>::Identity();
  uboData.m_view = mathfu::Matrix<float, 4, 4>::LookAt(mathfu::Vector<float, 3>(0.5f, 0.5f, 0.0f), mathfu::Vector<float, 3>(0.5f, 0.5f, -6.0f), mathfu::Vector<float, 3>(0.0f, 1.0f, 0.0f), -1.0f);
  uboData.m_proj = mathfu::Matrix<float, 4, 4>::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f, -1.0f);

  // TODO(vasumahesh1):[Q]:Allocator?
  ApplicationRequirements applicationRequirements(m_mainAllocator);
  applicationRequirements.m_clearColor[0] = 0.2f;
  applicationRequirements.m_clearColor[1] = 0.2f;
  applicationRequirements.m_clearColor[2] = 0.2f;
  applicationRequirements.m_uniformBuffers.Reserve(1);
  applicationRequirements.m_uniformBuffers.PushBack(std::make_pair(ShaderStage::Vertex,
                                                                   UniformBufferDesc{sizeof(UniformBufferData), 1, uboBinding}));

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            m_window->GetSwapChainRequirements(), m_mainAllocator, m_drawableAllocator,
                                            *m_window);
  m_renderer->SetDrawablePoolCount(1);

  auto vertShader = RenderSystem::
    CreateShader(*m_renderer, "Shaders/" + m_renderer->GetRenderingAPI() + "/test.vertex", log_AppRenderer);
  vertShader->SetStage(ShaderStage::Vertex);


  auto pixelShader = RenderSystem::
    CreateShader(*m_renderer, "Shaders/" + m_renderer->GetRenderingAPI() + "/test.pixel", log_AppRenderer);
  pixelShader->SetStage(ShaderStage::Pixel);

  DrawablePoolCreateInfo poolInfo = {};
  poolInfo.m_byteSize             = 4096;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_numShaders         = 2;
  DrawablePool& pool              = m_renderer->CreateDrawablePool(poolInfo);

  pool.AddShader(*vertShader);
  pool.AddShader(*pixelShader);

  Slot vertexDataSlot      = {};
  vertexDataSlot.m_binding = 0;
  vertexDataSlot.m_rate    = BufferUsageRate::PerVertex;

  Vector<RawStorageFormat> vertexStride = Vector<RawStorageFormat>(ContainerExtent{2, 2}, allocatorTemporary);
  vertexStride[0]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  vertexStride[1]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  pool.AddBufferBinding(vertexDataSlot, vertexStride);

  Vector<Vertex> vertexData = Vector<Vertex>({
    Vertex{{0, 0, 1, 1}, {1, 0, 0, 1}},
    Vertex{{1, 0, 1, 1}, {0, 1, 0, 1}},
    Vertex{{1, 1, 1, 1}, {0, 0, 1, 1}},
    Vertex{{0, 1, 1, 1}, {1, 1, 1, 1}}
  }, allocatorTemporary);

  Vector<U32> indexData = Vector<U32>({
    0, 2, 1,
    2, 0, 3
  }, allocatorTemporary);

  const auto bufferStart = reinterpret_cast<U8*>(vertexData.Data()); // NOLINT
  const auto indexBufferStart = reinterpret_cast<U8*>(indexData.Data()); // NOLINT
  const auto uboDataBuffer = reinterpret_cast<U8*>(&uboData); // NOLINT

  // Create Drawable from Pool
  Drawable& drawable = pool.CreateDrawable();
  drawable.SetDrawMode(DrawType::InstancedIndexed);
  drawable.SetIndexCount(indexData.GetSize());
  drawable.SetVertexCount(vertexData.GetSize());
  drawable.SetInstanceCount(1);
  drawable.SetUniformCount(1);
  drawable.SetIndexFormat(RawStorageFormat::R32_UNORM);

  drawable.SetInstanceDataCount(0);
  drawable.SetVertexDataCount(1);
  drawable.SetUniformDataCount(1);

  drawable.AddVertexData(vertexDataSlot, bufferStart, vertexData.GetSize() * sizeof(Vertex));
  drawable.SetIndexData(indexBufferStart, indexData.GetSize() * sizeof(U32));
  drawable.AddUniformData(uboDataBuffer, sizeof(UniformBufferData), 0);

  // All Drawables Done
  m_renderer->Submit();

  LOG_INF(log_AppRenderer, LOG_LEVEL, "Initialized AppRenderer");
}

void AppRenderer::WindowUpdate() {
  m_renderer->RenderFrame();
}

void AppRenderer::Run() const {
  LOG_INF(log_AppRenderer, LOG_LEVEL, "Running AppRenderer");
  m_renderer->RenderFrame();
  // m_renderer->RenderFrame();
  // m_renderer->RenderFrame();
  // m_renderer->RenderFrame();
  // m_renderer->SnapshotFrame("./temp.data");

  // m_window->StartListening();
}

void AppRenderer::Destroy() const {
  m_window->Destroy();
}
} // namespace Azura
