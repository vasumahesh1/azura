#include "TestZone/AppRenderer.h"
#include "Core/RawStorageFormat.h"
#include "Generic/RenderSystem.h"
#include "Generic/Shader.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Math/Transform.h"
#include "Math/Icosphere.h"
#include "Vulkan/VkTypeMapping.h"

namespace Azura {
using namespace Containers; // NOLINT
using namespace Math;       // NOLINT

#define VERTEX_SLOT 0

struct Vertex {
  float m_pos[4];
  float m_col[4];
  float m_uv[2];
};

struct QuadVertex {
  float m_pos[4];
  float m_uv[2];
};

struct Instance {
  float m_pos[4];
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
  const Matrix4f view       = Transform::LookAt(Vector3f(0.5f, 0.5f, 1.0f), Vector3f(0.5f, 0.5f, 4.0f),
                                                Vector3f(0.0f, 1.0f, 0.0f));
  const Matrix4f proj = Transform::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

  uboData.m_viewProj = proj * view;

  uboData.m_modelInvTranspose = uboData.m_model.Inverse().Transpose();

  // TODO(vasumahesh1):[Q]:Allocator?
  ApplicationRequirements applicationRequirements = {};
  applicationRequirements.m_clearColor[0]         = 0.2f;
  applicationRequirements.m_clearColor[1]         = 0.2f;
  applicationRequirements.m_clearColor[2]         = 0.2f;

  TextureRequirements textureRequirements = {};
  textureRequirements.m_maxCount          = 1;
  textureRequirements.m_poolSize          = 0x400000; // 4MB

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(3, allocatorTemporary);
  const U32 UBO_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex });
  const U32 SAMPLER_SLOT = descriptorRequirements.AddDescriptor({DescriptorType::Sampler, ShaderStage::Pixel});
  const U32 BASIC_TEXTURE_SLOT = descriptorRequirements.AddDescriptor({DescriptorType::SampledImage, ShaderStage::Pixel, DescriptorBinding::Same});

  ShaderRequirements shaderRequirements = ShaderRequirements(2, allocatorTemporary);
  const U32 VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "TestZone.vs", AssetLocation::Shaders });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "TestZone.ps", AssetLocation::Shaders });

  const U32 DEF_VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "TestZone.Deferred.vs", AssetLocation::Shaders });
  const U32 DEF_PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "TestZone.Deferred.ps", AssetLocation::Shaders });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(1, 2, allocatorTemporary);
  const U32 COLOR_TARGET_1 = renderPassRequirements.AddTarget({RawStorageFormat::R32G32B32A32_FLOAT});

  const U32 GBUFFER_PASS = renderPassRequirements.AddPass({
    {VERTEX_SHADER_ID, PIXEL_SHADER_ID},  // SHADERS
    {},                                   // INPUT TARGETS
    {COLOR_TARGET_1},     // OUTPUT TARGETS
    {UBO_SLOT}                            // DESCRIPTORS
  });

  const U32 SHADING_PASS = renderPassRequirements.AddPass({
    {DEF_VERTEX_SHADER_ID, DEF_PIXEL_SHADER_ID},
    {{COLOR_TARGET_1, ShaderStage::Pixel}},
    {PRESENT_TARGET}, // END OF RENDERING
    {SAMPLER_SLOT}
  });

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            m_window->GetSwapChainRequirements(), renderPassRequirements,
                                            descriptorRequirements, shaderRequirements, m_mainAllocator, m_drawableAllocator,
                                            *m_window);
  m_renderer->SetDrawablePoolCount(2);

  m_textureManager = RenderSystem::CreateTextureManager(*m_renderer, textureRequirements, log_AppRenderer);

  const U32 nocturnalTexture = m_textureManager->Load("Textures/Nocturnal.jpg");

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = 0x400000;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_numShaders           = 2;
  poolInfo.m_drawType             = DrawType::InstancedIndexed;
  poolInfo.m_renderPasses = {{GBUFFER_PASS}, allocatorTemporary};
  poolInfo.m_vertexDataSlots      = {
    {
      {VERTEX_SLOT, BufferUsageRate::PerVertex}
    },
    allocatorTemporary
  };

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  const Vector<RawStorageFormat> vertexStride = Vector<RawStorageFormat>({
    RawStorageFormat::R32G32B32A32_FLOAT, // Pos
    RawStorageFormat::R32G32B32A32_FLOAT, // Color
    RawStorageFormat::R32G32_FLOAT        // UV
  }, allocatorTemporary);

  pool.AddBufferBinding(VERTEX_SLOT, vertexStride);

  const TextureDesc* desc = m_textureManager->GetInfo(nocturnalTexture);
  VERIFY_TRUE(log_AppRenderer, desc != nullptr, "Texture Description was Null");

  pool.BindTextureData(BASIC_TEXTURE_SLOT, *desc, m_textureManager->GetData(nocturnalTexture));
  pool.BindSampler(SAMPLER_SLOT, {});

  Vector<Vertex> vertexData = Vector<Vertex>({
    Vertex{{0, 0, 1, 1}, {1, 0, 0, 1}, {0, 0}},
    Vertex{{1, 0, 1, 1}, {0, 1, 0, 1}, {1, 0}},
    Vertex{{1, 1, 1, 1}, {0, 0, 1, 1}, {1, 1}},
    Vertex{{0, 1, 1, 1}, {1, 1, 1, 1}, {0, 1}}
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
  pool.BindVertexData(drawableId, VERTEX_SLOT, bufferStart, vertexData.GetSize() * sizeof(Vertex));
  pool.SetIndexData(drawableId, indexBufferStart, indexData.GetSize() * sizeof(U32));
  pool.BindUniformData(drawableId, UBO_SLOT, uboDataBuffer, sizeof(UniformBufferData));


  // QUAD
  DrawablePoolCreateInfo quadPoolInfo = {allocatorTemporary};
  quadPoolInfo.m_byteSize             = 0x400000;
  quadPoolInfo.m_numDrawables         = 1;
  quadPoolInfo.m_numShaders           = 2;
  quadPoolInfo.m_drawType             = DrawType::InstancedIndexed;
  quadPoolInfo.m_cullMode             = CullMode::None;
  quadPoolInfo.m_renderPasses = {{SHADING_PASS}, allocatorTemporary};
  quadPoolInfo.m_vertexDataSlots      = {
    {
      {VERTEX_SLOT, BufferUsageRate::PerVertex}
    },
    allocatorTemporary
  };
  
  DrawablePool& quadPool = m_renderer->CreateDrawablePool(quadPoolInfo);
  
  const Vector<RawStorageFormat> quadStride = Vector<RawStorageFormat>({
    RawStorageFormat::R32G32B32A32_FLOAT, // Pos
    RawStorageFormat::R32G32_FLOAT // UV
    }, allocatorTemporary);
  
  quadPool.AddBufferBinding(VERTEX_SLOT, quadStride);
  
  quadPool.BindSampler(SAMPLER_SLOT, {});
  
  Vector<QuadVertex> quadVertexData = Vector<QuadVertex>({
    QuadVertex{{-1, -1, 0, 1}, {0, 0}},
    QuadVertex{{1, -1, 0, 1}, {1, 0}},
    QuadVertex{{1, 1, 0, 1}, {1, 1}},
    QuadVertex{{-1, 1, 0, 1}, {0, 1}}
    }, allocatorTemporary);
  
  Vector<U32> quadIndexData = Vector<U32>({
    0, 1, 2,
    2, 3, 0
    }, allocatorTemporary);
  
  const auto quadBufferStart      = reinterpret_cast<U8*>(quadVertexData.Data()); // NOLINT
  const auto quadIndexStart = reinterpret_cast<U8*>(quadIndexData.Data());  // NOLINT
  
                                                                          // Create Drawable from Pool
  createInfo = DrawableCreateInfo{};
  createInfo.m_vertexCount      = quadVertexData.GetSize();
  createInfo.m_indexCount       = quadIndexData.GetSize();
  createInfo.m_instanceCount    = 1;
  createInfo.m_indexType        = RawStorageFormat::R32_UINT;
  
  const auto quadId = quadPool.CreateDrawable(createInfo);
  quadPool.BindVertexData(quadId, VERTEX_SLOT, quadBufferStart, quadVertexData.GetSize() * sizeof(QuadVertex));
  quadPool.SetIndexData(quadId, quadIndexStart, quadIndexData.GetSize() * sizeof(U32));
  


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
