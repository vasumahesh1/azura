#include "TestZone/AppRenderer.h"
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

struct VertexWithUV {
  float m_pos[4];
  float m_uv[2];
};

struct LightData
{
  Vector4f m_lightPos;
};

AppRenderer::AppRenderer()
  : m_mainBuffer(16384),
    m_mainAllocator(m_mainBuffer, 8192),
    m_drawableAllocator(m_mainBuffer, 8192),
    m_camera(1280, 720),
    m_clothVertices(m_mainAllocator),
    log_AppRenderer(Log("AppRenderer")) {
}

void AppRenderer::Initialize() {
  LOG_INF(log_AppRenderer, LOG_LEVEL, "Starting Init of AppRenderer");

  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);
  m_window = RenderSystem::CreateApplicationWindow("TestZone", 1280, 720);

  m_window->SetUpdateCallback([this](float deltaTime)
  {
    WindowUpdate(deltaTime);
  });

  m_window->SetKeyEventCallback([this](KeyEvent evt)
  {
    m_camera.OnKeyEvent(evt);
  });

  m_window->SetMouseEventCallback([this](MouseEvent evt)
  {
    UNUSED(evt);
    // Disable as it has bugs
    // m_camera.OnMouseEvent(e);
  });

  m_camera.SetZoom(10);
  m_camera.RotateAboutRight(-30);

  VERIFY_TRUE(log_AppRenderer, m_window->Initialize(), "Cannot Initialize Window");

  ApplicationInfo appInfo;
  appInfo.m_name    = "TestZone";
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  m_camera.Recompute();

  m_sceneUBO = {};
  m_sceneUBO.m_model           = Matrix4f::Identity();
  m_sceneUBO.m_viewProj = m_camera.GetViewProjMatrix();
  m_sceneUBO.m_invViewProj = m_camera.GetInvViewProjMatrix();
  m_sceneUBO.m_invProj = m_camera.GetProjMatrix().Inverse();

  LightData lightData = {};

  // TODO(vasumahesh1):[Q]:Allocator?
  const ApplicationRequirements applicationRequirements = {};

  TextureRequirements textureRequirements = {};
  textureRequirements.m_maxCount          = 1;
  textureRequirements.m_poolSize          = 0x400000; // 4MB

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(2, 2, allocatorTemporary);
  const U32 UBO_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex });
  const U32 LIGHT_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex });

  const U32 UBO_SET = descriptorRequirements.AddSet({ UBO_SLOT });
  const U32 LIGHT_SET = descriptorRequirements.AddSet({ LIGHT_SLOT });

  ShaderRequirements shaderRequirements = ShaderRequirements(2, allocatorTemporary);
  const U32 VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "Cloth.vs", AssetLocation::Shaders });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "Cloth.ps", AssetLocation::Shaders });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(1, 2, 0, allocatorTemporary);
  renderPassRequirements.m_maxPools = 1;

  const U32 SINGLE_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID},  // SHADERS
    PipelinePassCreateInfo::InputTargets{},                                    // INPUT TARGETS
    PipelinePassCreateInfo::InputBuffers{},                                    // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{},                     // OUTPUT TARGETS
    PipelinePassCreateInfo::DescriptorSets{UBO_SET, LIGHT_SET},   // DESCRIPTORS
    ClearData{{0.2f, 0.2f, 0.2f, 1.0f}, 1.0f, 0}
  });

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            m_window->GetSwapChainRequirements(), renderPassRequirements,
                                            descriptorRequirements, shaderRequirements, m_mainAllocator, m_drawableAllocator,
                                            *m_window);

  m_textureManager = RenderSystem::CreateTextureManager(textureRequirements);

  IcoSphere sphere(4);

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = 0x400000;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_renderPasses = {{SINGLE_PASS}, allocatorTemporary};
  poolInfo.m_drawType             = DrawType::InstancedIndexed;

  const auto VERTEX_SLOT = poolInfo.AddInputSlot({ BufferUsageRate::PerVertex, { {"POSITION", RawStorageFormat::R32G32B32A32_FLOAT} } });
  const auto NORMAL_SLOT = poolInfo.AddInputSlot({ BufferUsageRate::PerVertex, { {"NORMAL", RawStorageFormat::R32G32B32A32_FLOAT} } });

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  m_clothVertices.Reserve(4);
  m_clothVertices.PushBack({0, 1, 0, 1});
  m_clothVertices.PushBack({1, 1, 0, 1});
  m_clothVertices.PushBack({1, 1, 1, 1});
  m_clothVertices.PushBack({0, 1, 1, 1});

  Vector<Vector4f> normalData = Vector<Vector4f>({
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1}
    }, allocatorTemporary);

  Vector<U32> indexData = Vector<U32>({
    0, 1, 2,
    2, 3, 0
    }, allocatorTemporary);

  const auto bufferStart      = reinterpret_cast<U8*>(m_clothVertices.Data()); // NOLINT
  const auto normalStart      = reinterpret_cast<U8*>(normalData.Data()); // NOLINT
  const auto indexBufferStart = reinterpret_cast<U8*>(indexData.Data());  // NOLINT
  const auto uboDataBuffer    = reinterpret_cast<U8*>(&m_sceneUBO);          // NOLINT
  const auto lightDataBuffer    = reinterpret_cast<U8*>(&lightData);          // NOLINT
                                                                          // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = m_clothVertices.GetSize();
  createInfo.m_indexCount       = indexData.GetSize();
  createInfo.m_instanceCount    = 1;
  createInfo.m_indexType        = RawStorageFormat::R32_UINT;

  const auto drawableId = pool.CreateDrawable(createInfo);
  pool.BindVertexData(drawableId, VERTEX_SLOT, bufferStart, m_clothVertices.GetSize() * sizeof(Vector4f));
  pool.BindVertexData(drawableId, NORMAL_SLOT, normalStart, normalData.GetSize() * sizeof(Vector4f));
  pool.SetIndexData(drawableId, indexBufferStart, indexData.GetSize() * sizeof(U32));
  pool.BindUniformData(drawableId, UBO_SLOT, uboDataBuffer, sizeof(SceneUBO));
  pool.BindUniformData(drawableId, LIGHT_SLOT, lightDataBuffer, sizeof(LightData));

  m_renderPass.m_sceneUBOSlot = UBO_SLOT;
  m_renderPass.m_clothId = drawableId;

  m_mainPool = &pool;

  // All Drawables Done
  m_renderer->Submit();

  LOG_INF(log_AppRenderer, LOG_LEVEL, "Initialized AppRenderer");
}

void AppRenderer::WindowUpdate(float timeDelta) {
  m_camera.Update(timeDelta);

  for (auto& vertex : m_clothVertices)
  {
    vertex[1] -= (timeDelta * 0.2f);
  }

  m_sceneUBO.m_viewProj = m_camera.GetViewProjMatrix();
  m_sceneUBO.m_invViewProj = m_camera.GetInvViewProjMatrix();
  m_sceneUBO.m_invProj = m_camera.GetProjMatrix().Inverse();
  
  const auto vbStart      = reinterpret_cast<U8*>(m_clothVertices.Data()); // NOLINT
  const auto uboDataBuffer       = reinterpret_cast<U8*>(&m_sceneUBO);        // NOLINT

  m_mainPool->BeginUpdates();
  m_mainPool->UpdateUniformData(m_renderPass.m_clothId, m_renderPass.m_sceneUBOSlot, uboDataBuffer, sizeof(SceneUBO));
  m_mainPool->UpdateVertexData(m_renderPass.m_clothId, m_renderPass.m_sceneUBOSlot, vbStart, m_clothVertices.GetSize() * sizeof(Vector4f));
  m_mainPool->SubmitUpdates();

  m_renderer->RenderFrame();
}

void AppRenderer::Run() const {
  LOG_INF(log_AppRenderer, LOG_LEVEL, "Running AppRenderer");
  m_window->StartListening();
}

void AppRenderer::Destroy() const {
  m_window->Destroy();
}
} // namespace Azura
