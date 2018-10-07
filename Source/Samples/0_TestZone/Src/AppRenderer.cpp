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
#define UBO_SLOT 1
#define SAMPLER_SLOT 2
#define BASIC_TEXTURE_SLOT 3

struct Vertex {
  float m_pos[4];
  float m_col[4];
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
  const Matrix4f view       = Transform::LookAt(Vector3f(0.5f, 0.5f, 1.0f), Vector3f(0.5f, 0.5f, -4.0f),
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
  textureRequirements.m_maxCount = 1;
  textureRequirements.m_poolSize = 0x400000; // 4MB

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            m_window->GetSwapChainRequirements(), m_mainAllocator, m_drawableAllocator,
                                            *m_window);
  m_renderer->SetDrawablePoolCount(1);

  m_textureManager = RenderSystem::CreateTextureManager(*m_renderer, textureRequirements, log_AppRenderer);

  const U32 nocturnalTexture = m_textureManager->Load("Textures/Nocturnal.jpg");

  auto vertShader = RenderSystem::CreateShader(*m_renderer,
                                               "Shaders/" + m_renderer->GetRenderingAPI() + "/TestZone.vs",
                                               log_AppRenderer);
  vertShader->SetStage(ShaderStage::Vertex);

  auto pixelShader = RenderSystem::CreateShader(*m_renderer,
                                                "Shaders/" + m_renderer->GetRenderingAPI() + "/TestZone.ps",
                                                log_AppRenderer);
  pixelShader->SetStage(ShaderStage::Pixel);

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = 0x400000;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_numShaders           = 2;
  poolInfo.m_drawType             = DrawType::InstancedIndexed;
  poolInfo.m_vertexDataSlots      = {
    {
      {VERTEX_SLOT, BufferUsageRate::PerVertex}
    },
    allocatorTemporary
  };

  poolInfo.m_descriptorSlots = {
    {
      {UBO_SLOT, DescriptorType::UniformBuffer, ShaderStage::Vertex},
      {SAMPLER_SLOT, DescriptorType::Sampler, ShaderStage::Pixel},
      {BASIC_TEXTURE_SLOT, DescriptorType::SampledImage, ShaderStage::Pixel, DescriptorBinding::Same}
    },
    allocatorTemporary
  };

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  pool.AddShader(*vertShader);
  pool.AddShader(*pixelShader);

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
