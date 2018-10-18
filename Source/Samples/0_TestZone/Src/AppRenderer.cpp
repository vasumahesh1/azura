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

struct UniformBufferData {
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_viewProj;
};

AppRenderer::AppRenderer()
  : m_mainBuffer(16384),
    m_mainAllocator(m_mainBuffer, 8192),
    m_drawableAllocator(m_mainBuffer, 8192),
    log_AppRenderer(Log("AppRenderer")) {
}

void AppRenderer::Initialize() {
  LOG_INF(log_AppRenderer, LOG_LEVEL, "Starting Init of AppRenderer");

  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);
  m_window = RenderSystem::CreateApplicationWindow("TestZone", 1280, 720);

  m_window->SetUpdateCallback([this]()
  {
    WindowUpdate();
  });

  VERIFY_TRUE(log_AppRenderer, m_window->Initialize(), "Cannot Initialize Window");

  ApplicationInfo appInfo;
  appInfo.m_name    = "TestZone";
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  UniformBufferData uboData = {};
  uboData.m_model           = Matrix4f::Identity();
  const auto view           = Transform::LookAt(Vector3f(0.5f, 0.5f, 0.0f), Vector3f(0.5f, 0.5f, 6.0f),
    Vector3f(0.0f, 1.0f, 0.0f));
  const auto proj = Transform::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

  uboData.m_viewProj          = proj * view;
  uboData.m_modelInvTranspose = uboData.m_model.Inverse().Transpose();

  // TODO(vasumahesh1):[Q]:Allocator?
  const ApplicationRequirements applicationRequirements = {};

  TextureRequirements textureRequirements = {};
  textureRequirements.m_maxCount          = 1;
  textureRequirements.m_poolSize          = 0x400000; // 4MB

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(3, allocatorTemporary);
  const U32 UBO_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex });

  ShaderRequirements shaderRequirements = ShaderRequirements(2, allocatorTemporary);
  const U32 VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "BasicRenderTest.vs", AssetLocation::Shaders });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "BasicRenderTest.ps", AssetLocation::Shaders });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(0, 1, allocatorTemporary);

  const U32 SINGLE_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID},  // SHADERS
    PipelinePassCreateInfo::Inputs{},                                    // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{},                     // OUTPUT TARGETS
    PipelinePassCreateInfo::Descriptors{UBO_SLOT},                        // DESCRIPTORS
    ClearData{{0.2f, 0.2f, 0.2f, 1.0f}, 1.0f, 0}
  });

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            m_window->GetSwapChainRequirements(), renderPassRequirements,
                                            descriptorRequirements, shaderRequirements, m_mainAllocator, m_drawableAllocator,
                                            *m_window);

  m_textureManager = RenderSystem::CreateTextureManager(textureRequirements);

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = 0x400000;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_drawType             = DrawType::InstancedIndexed;
  poolInfo.m_renderPasses = {{SINGLE_PASS}, allocatorTemporary};

  const U32 VERTEX_SLOT = poolInfo.AddInputSlot({ BufferUsageRate::PerVertex, { {"POSITION", RawStorageFormat::R32G32B32A32_FLOAT}, {"COLOR", RawStorageFormat::R32G32B32A32_FLOAT}} });

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  Vector<Vertex> vertexData = Vector<Vertex>({
    Vertex{{0, 0, 1, 1}, {1, 0, 0, 1}},
    Vertex{{1, 0, 1, 1}, {0, 1, 0, 1}},
    Vertex{{1, 1, 1, 1}, {0, 0, 1, 1}},
    Vertex{{0, 1, 1, 1}, {1, 1, 1, 1}}
    }, allocatorTemporary);

  Vector<U32> indexData = Vector<U32>({
    0, 1, 2,
    2, 3, 0
    }, allocatorTemporary);

  uboData.m_viewProj          = proj * view;
  uboData.m_modelInvTranspose = uboData.m_model.Inverse().Transpose();

  const auto bufferStart      = reinterpret_cast<U8*>(vertexData.Data()); // NOLINT
  const auto indexBufferStart = reinterpret_cast<U8*>(indexData.Data());  // NOLINT
  const auto uboDataBuffer    = reinterpret_cast<U8*>(&uboData);          // NOLINT
                                                                          // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = vertexData.GetSize();
  createInfo.m_indexCount       = indexData.GetSize();
  createInfo.m_instanceCount    = 1;
  createInfo.m_indexType        = RawStorageFormat::R32_UINT;

  const auto drawableId = pool.CreateDrawable(createInfo);
  pool.BindVertexData(drawableId, VERTEX_SLOT, bufferStart, vertexData.GetSize() * sizeof(Vertex));
  pool.SetIndexData(drawableId, indexBufferStart, indexData.GetSize() * sizeof(U32));
  pool.BindUniformData(drawableId, UBO_SLOT, uboDataBuffer, sizeof(UniformBufferData));

  // All Drawables Done
  m_renderer->Submit();

  LOG_INF(log_AppRenderer, LOG_LEVEL, "Initialized AppRenderer");
}

void AppRenderer::WindowUpdate() {
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
