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

  m_window->SetUpdateCallback([this](float timeSinceLastFrame)
  {
    UNUSED(timeSinceLastFrame);
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

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(4, 4, allocatorTemporary);
  const U32 UBO_SLOT                            = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Vertex
  });
  const U32 SAMPLER_SLOT = descriptorRequirements.AddDescriptor({
    DescriptorType::Sampler, ShaderStage::Pixel
  });

  const U32 UBO_SET     = descriptorRequirements.AddSet({UBO_SLOT});
  const U32 SAMPLER_SET = descriptorRequirements.AddSet({SAMPLER_SLOT});

  ShaderRequirements shaderRequirements = ShaderRequirements(4, allocatorTemporary);
  const U32 COMPUTE_SHADER_ID            = shaderRequirements.AddShader({
    ShaderStage::Compute, "BasicCompute.cs", AssetLocation::Shaders
    });

  const U32 DEF_VERTEX_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Vertex, "BasicCompute.vs", AssetLocation::Shaders
  });
  const U32 DEF_PIXEL_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Pixel, "BasicCompute.ps", AssetLocation::Shaders
  });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(1, 2, allocatorTemporary);
  renderPassRequirements.m_maxPools             = 2;

  const U32 COLOR_TARGET_1 = renderPassRequirements.AddTarget({RawStorageFormat::R32G32B32A32_FLOAT, 32, 32});

  const U32 COMPUTE_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{COMPUTE_SHADER_ID},
    PipelinePassCreateInfo::Inputs{},
    PipelinePassCreateInfo::Outputs{COLOR_TARGET_1},
    PipelinePassCreateInfo::DescriptorSets{},
    ClearData{{0.2f, 0.2f, 0.2f, 1.0f}, 1.0f, 0},
    BlendState{},
    RenderPassType::Compute
    });

  const U32 SHADE_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{DEF_VERTEX_SHADER_ID, DEF_PIXEL_SHADER_ID},        // SHADERS
    PipelinePassCreateInfo::Inputs{{COLOR_TARGET_1, ShaderStage::Pixel}},                                          // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{},
    PipelinePassCreateInfo::DescriptorSets{UBO_SET, SAMPLER_SET},
    ClearData{{0.2f, 0.2f, 0.2f, 1.0f}, 1.0f, 0}
  });

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            m_window->GetSwapChainRequirements(), renderPassRequirements,
                                            descriptorRequirements, shaderRequirements, m_mainAllocator,
                                            m_drawableAllocator,
                                            *m_window);

  m_textureManager = RenderSystem::CreateTextureManager(textureRequirements);

  // COMPUTE

  ComputePoolCreateInfo computePoolCreateInfo = {allocatorTemporary};
  computePoolCreateInfo.m_byteSize = 0x400000;
  computePoolCreateInfo.m_computePasses = { {COMPUTE_PASS}, allocatorTemporary };
  computePoolCreateInfo.m_launchDims = ThreadGroupDimensions{ 50, 50, 1 };


  ComputePool& computePool = m_renderer->CreateComputePool(computePoolCreateInfo);
  UNUSED(computePool);


  // DRAWABLE

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = 0x400000;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_renderPasses         = {{SHADE_PASS}, allocatorTemporary};
  poolInfo.m_drawType             = DrawType::InstancedIndexed;

  const auto VERTEX_SLOT = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex,
    {{"POSITION", RawStorageFormat::R32G32B32A32_FLOAT}, {"UV", RawStorageFormat::R32G32_FLOAT}}
  });

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  pool.BindSampler(SAMPLER_SLOT, {});

  Vector<VertexWithUV> vertexData = Vector<VertexWithUV>({
    VertexWithUV{{0, 0, 1, 1}, {0, 0}},
    VertexWithUV{{1, 0, 1, 1}, {1, 0}},
    VertexWithUV{{1, 1, 1, 1}, {1, 1}},
    VertexWithUV{{0, 1, 1, 1}, {0, 1}}
  }, allocatorTemporary);

  Vector<U32> indexData = Vector<U32>({
    0, 1, 2,
    2, 3, 0
  }, allocatorTemporary);

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
  pool.BindVertexData(drawableId, VERTEX_SLOT, bufferStart, vertexData.GetSize() * sizeof(VertexWithUV));
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
