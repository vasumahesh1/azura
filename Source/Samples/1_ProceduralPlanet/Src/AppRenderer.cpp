#include "ProceduralPlanet/AppRenderer.h"
#include "Core/RawStorageFormat.h"
#include "Generic/RenderSystem.h"
#include "Generic/Shader.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Math/Transform.h"
#include "Math/Icosphere.h"

namespace Azura {
using namespace Containers; // NOLINT
using namespace Math;       // NOLINT

struct Vertex {
  float m_pos[4];
  float m_col[4];
};

struct Instance {
  float m_pos[4];
};

struct UniformBufferData
{
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_viewProj;
};

struct ShaderControls { // NOLINT
  float m_shoreLevel{0.5f};
  float m_elevation{0.5f};
  float m_noiseScale{0.5f};
  float pad1;

  Vector4f m_lightPos{ 0, 0, 15, 1 };
  Vector4f m_eye{ 0.0f, 0.0f, 4.0f, 1.0f };

  Color4f m_waterControls{ 0.5f, 0.65f, 0, 0 };
  Color4f m_waterColor{ 21.0, 92.0, 158.0, 1.0f };
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

  Slot normalDataSlot      = {};
  normalDataSlot.m_binding = 1;
  normalDataSlot.m_rate    = BufferUsageRate::PerVertex;

  Slot shaderControlSlot      = {};
  shaderControlSlot.m_binding = 0;

  Slot uniformSlot      = {};
  uniformSlot.m_binding = 1;

  ShaderControls shaderControls{};

  UniformBufferData uboData = {};
  uboData.m_model           = Matrix4f::Identity();
  const Matrix4f view            = Transform::LookAt(Vector3f(0.0f, 0.0f, 1.0f), shaderControls.m_eye.xyz(),
                                                Vector3f(0.0f, 1.0f, 0.0f));
  const Matrix4f proj = Transform::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

  uboData.m_viewProj = proj * view;

  uboData.m_modelInvTranspose = uboData.m_model.Inverse().Transpose();

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

  IcoSphere sphere(6);

  DrawablePoolCreateInfo poolInfo(allocatorTemporary);
  poolInfo.m_byteSize                    = sphere.TotalDataSize() + 0xFFFF;
  poolInfo.m_numDrawables                = 1;
  poolInfo.m_numShaders                  = 2;
  poolInfo.m_drawType                    = DrawType::InstancedIndexed;
  poolInfo.m_slotInfo.m_numVertexSlots   = 2;
  poolInfo.m_slotInfo.m_numInstanceSlots = 0;

  // UBO Info
  poolInfo.m_uniformBuffers.Reserve(2);
  poolInfo.m_uniformBuffers.PushBack(std::make_pair(shaderControlSlot,
    UniformBufferDesc{
      sizeof(ShaderControls), 1
    }));

  poolInfo.m_uniformBuffers.PushBack(std::make_pair(uniformSlot,
    UniformBufferDesc{
      sizeof(UniformBufferData), 1
    }));

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  pool.AddShader(*vertShader);
  pool.AddShader(*pixelShader);

  Vector<RawStorageFormat> vertexStride = Vector<RawStorageFormat>(1, allocatorTemporary);
  vertexStride.PushBack(sphere.GetVertexFormat());
  pool.AddBufferBinding(vertexDataSlot, vertexStride);

  pool.AddBufferBinding(normalDataSlot, {{ sphere.GetNormalFormat() }, allocatorTemporary });

  const auto uboDataBuffer = reinterpret_cast<U8*>(&uboData); // NOLINT
  const auto shaderControlBuffer = reinterpret_cast<U8*>(&shaderControls); // NOLINT

  // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = sphere.GetVertexCount();
  createInfo.m_indexCount       = sphere.GetIndexCount();
  createInfo.m_instanceCount    = 1;
  createInfo.m_indexType        = sphere.GetIndexFormat();

  const auto drawableId = pool.CreateDrawable(createInfo);
  pool.BindVertexData(drawableId, vertexDataSlot, sphere.VertexData(), sphere.VertexDataSize());
  pool.BindVertexData(drawableId, normalDataSlot, sphere.NormalData(), sphere.NormalDataSize());
  pool.SetIndexData(drawableId, sphere.IndexData(), sphere.IndexDataSize());
  pool.BindUniformData(drawableId, uniformSlot, uboDataBuffer, sizeof(UniformBufferData));
  pool.BindUniformData(drawableId, shaderControlSlot, shaderControlBuffer, sizeof(ShaderControls));

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
