#include "App/AppRenderer.h"
#include "Core/RawStorageFormat.h"
#include "Generic/RenderSystem.h"
#include "Generic/Shader.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Math/Transform.h"
#include "Math/Icosphere.h"

#include "AssetManager/GLTFLoader.h"

namespace Azura {
using namespace Containers; // NOLINT
using namespace Math;       // NOLINT

constexpr U32 NUM_LIGHTS = 10;

AppRenderer::AppRenderer()
  : 
  m_forwardRenderer(),
    m_mainBuffer(16384),
    m_mainAllocator(m_mainBuffer, 8192),
    m_drawableAllocator(m_mainBuffer, 8192),
    m_lightTexture(NUM_LIGHTS, 7, m_mainAllocator),
    m_camera(1280, 720, -90, -45, 10),
    log_AppRenderer(Log("AppRenderer")) {
}

void AppRenderer::LoadAssets() const {
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 4096);

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = 0xF00000;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_renderPasses         = {{m_forwardRenderer.m_passId}, allocatorTemporary};
  poolInfo.m_drawType             = DrawType::InstancedIndexed;
  poolInfo.m_cullMode             = CullMode::None;

  const auto VERTEX_SLOT = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"POSITION", RawStorageFormat::R32G32B32_FLOAT}}
  });
  const auto NORMAL_SLOT = poolInfo.AddInputSlot({
    BufferUsageRate::PerVertex, {{"NORMAL", RawStorageFormat::R32G32B32_FLOAT}}
  });
  const auto UV_SLOT = poolInfo.AddInputSlot({BufferUsageRate::PerVertex, {{"UV", RawStorageFormat::R32G32_FLOAT}}});

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  // Pool Binds
  pool.BindSampler(m_forwardRenderer.m_sampSlot, {});
  // pool.BindSampler(m_forwardRenderer.m_lightSampSlot, m_lightSamplerDesc);

  // Bind Texture
  const U32 colorTexture = m_textureManager->Load("Meshes/sponza/color.png");
  const U32 normalTexture = m_textureManager->Load("Meshes/sponza/normal.png");

  const TextureDesc* colorDesc = m_textureManager->GetInfo(colorTexture);
  const TextureDesc* normalDesc = m_textureManager->GetInfo(normalTexture);

  VERIFY_TRUE(log_AppRenderer, colorDesc != nullptr, "Color Texture Description was Null");
  VERIFY_TRUE(log_AppRenderer, normalDesc != nullptr, "Normal Texture Description was Null");
  pool.BindTextureData(m_forwardRenderer.m_texSlot, *colorDesc, m_textureManager->GetData(colorTexture));
  pool.BindTextureData(m_forwardRenderer.m_normalSlot, *normalDesc, m_textureManager->GetData(normalTexture));

  const auto meshInterface = Azura::GLTFLoader::LoadFromJSON("sponza", AssetLocation::Meshes, log_AppRenderer);

  U32 vertexDataSize    = 0;
  U32 numVertices       = 0;
  const auto vertexData = meshInterface.GetPositionBuffer(0, 0, vertexDataSize, numVertices);

  U32 normalDataSize    = 0;
  U32 numNormals        = 0;
  const auto normalData = meshInterface.GetNormalBuffer(0, 0, normalDataSize, numNormals);

  U32 uvDataSize    = 0;
  U32 numUV         = 0;
  const auto uvData = meshInterface.GetUVBuffer(0, 0, uvDataSize, numUV);

  U32 indexDataSize    = 0;
  U32 numIndices       = 0;
  const auto indexData = meshInterface.GetIndexBuffer(0, 0, indexDataSize, numIndices);

  const auto vertexStart      = reinterpret_cast<const U8*>(vertexData.data()); // NOLINT
  const auto normalStart      = reinterpret_cast<const U8*>(normalData.data()); // NOLINT
  const auto uvStart          = reinterpret_cast<const U8*>(uvData.data());     // NOLINT
  const auto indexBufferStart = reinterpret_cast<const U8*>(indexData.data());  // NOLINT
  const auto uboDataBuffer    = reinterpret_cast<const U8*>(&m_sceneUBO);       // NOLINT

  // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = numVertices;
  createInfo.m_indexCount       = numIndices;
  createInfo.m_instanceCount    = 1;
  createInfo.m_indexType        = RawStorageFormat::R32_UINT;

  // Drawable Binds
  const auto drawableId = pool.CreateDrawable(createInfo);
  pool.BindVertexData(drawableId, VERTEX_SLOT, vertexStart, vertexDataSize);
  pool.BindVertexData(drawableId, NORMAL_SLOT, normalStart, normalDataSize);
  pool.BindVertexData(drawableId, UV_SLOT, uvStart, uvDataSize);
  pool.SetIndexData(drawableId, indexBufferStart, indexDataSize);
  pool.BindUniformData(drawableId, m_forwardRenderer.m_uboSlot, uboDataBuffer, sizeof(UniformBufferData));
}

void AppRenderer::Initialize() {
  LOG_INF(log_AppRenderer, LOG_LEVEL, "Starting Init of AppRenderer");

  m_lightSamplerDesc.m_filter = TextureFilter::MinMagMipPoint;
  m_lightSamplerDesc.m_addressModeU = TextureAddressMode::Clamp;
  m_lightSamplerDesc.m_addressModeV = TextureAddressMode::Clamp;
  m_lightSamplerDesc.m_addressModeW = TextureAddressMode::Clamp;

  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);
  m_window = RenderSystem::CreateApplicationWindow("TestZone", 1280, 720);

  m_window->SetUpdateCallback([this]()
  {
    WindowUpdate();
  });

  VERIFY_TRUE(log_AppRenderer, m_window->Initialize(), "Cannot Initialize Window");

  ApplicationInfo appInfo;
  appInfo.m_name = "TestZone";
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU = true;
  requirements.m_float64 = false;
  requirements.m_int64 = false;
  requirements.m_transferQueue = false;

  m_sceneUBO         = {};
  m_sceneUBO.m_model           = Matrix4f::Identity();
  m_sceneUBO.m_model(1, 3) = -5;

  m_sceneUBO.m_viewProj = m_camera.GetViewProjMatrix();
  m_sceneUBO.m_modelInvTranspose = m_sceneUBO.m_model.Inverse().Transpose();

  // TODO(vasumahesh1):[Q]:Allocator?
  const ApplicationRequirements applicationRequirements = {};

  TextureRequirements textureRequirements = {};
  textureRequirements.m_maxCount          = 10;
  textureRequirements.m_poolSize          = 0xF00000;

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(6, 4, allocatorTemporary);

  // UBs
  m_forwardRenderer.m_uboSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Vertex
  });

  // SAMPLERS
  m_forwardRenderer.m_sampSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::Sampler, ShaderStage::Pixel
    });

  // m_forwardRenderer.m_lightSampSlot = descriptorRequirements.AddDescriptor({
  //   DescriptorType::Sampler, ShaderStage::Pixel
  //   });

  // TEXTURES
  m_forwardRenderer.m_texSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::SampledImage, ShaderStage::Pixel
  });

  m_forwardRenderer.m_normalSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::SampledImage, ShaderStage::Pixel
    });

  // m_forwardRenderer.m_lightTexSlot = descriptorRequirements.AddDescriptor({
  //   DescriptorType::SampledImage, ShaderStage::Pixel
  //   });

  const U32 UBO_SET = descriptorRequirements.AddSet({ m_forwardRenderer.m_uboSlot });
  const U32 SAMPLER_SET = descriptorRequirements.AddSet({ m_forwardRenderer.m_sampSlot});
  const U32 TEXTURE_SET = descriptorRequirements.AddSet({ m_forwardRenderer.m_texSlot, m_forwardRenderer.m_normalSlot, /* m_forwardRenderer.m_lightTexSlot */ });
  // const U32 LIGHT_SAMPLER_SET = descriptorRequirements.AddSet({ m_forwardRenderer.m_lightSampSlot });

  ShaderRequirements shaderRequirements = ShaderRequirements(2, allocatorTemporary);
  const U32 VERTEX_SHADER_ID            = shaderRequirements.AddShader({
    ShaderStage::Vertex, "Forward.vs", AssetLocation::Shaders
  });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Pixel, "Forward.ps", AssetLocation::Shaders
  });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(1, 2, allocatorTemporary);
  renderPassRequirements.m_maxPools             = 1;

  m_forwardRenderer.m_passId = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID}, // SHADERS
    PipelinePassCreateInfo::Inputs{},                                   // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{},                                  // OUTPUT TARGETS
    PipelinePassCreateInfo::DescriptorSets{UBO_SET, SAMPLER_SET, /* LIGHT_SAMPLER_SET ,*/ TEXTURE_SET},                    // DESCRIPTORS
    ClearData{{0.2f, 0.2f, 0.2f, 1.0f}, 1.0f, 0}
  });

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            m_window->GetSwapChainRequirements(), renderPassRequirements,
                                            descriptorRequirements, shaderRequirements, m_mainAllocator,
                                            m_drawableAllocator,
                                            *m_window);

  m_textureManager = RenderSystem::CreateTextureManager(textureRequirements);

  LoadAssets();

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
