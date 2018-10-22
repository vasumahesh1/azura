#include "App/AppRenderer.h"

#include <random>
#include <cmath>

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
namespace {
  constexpr U32 NUM_LIGHTS = 5;
  const Vector3f LIGHT_MIN = {-14.0f, 0.0f, -6.0f };
  const Vector3f LIGHT_MAX = { 14.0f, 20.0f, 6.0f };
  constexpr float LIGHT_RADIUS = 5.0;
  // constexpr float LIGHT_DT = -0.03;

} // namespace

AppRenderer::AppRenderer()
  : m_mainPool(nullptr),
    m_sponzaId(0),
    m_forwardRenderer(),
    m_mainBuffer(16384),
    m_mainAllocator(m_mainBuffer, 8192),
    m_drawableAllocator(m_mainBuffer, 8192),
    m_lightTexture(NUM_LIGHTS, 7, m_mainAllocator),
    m_lights(NUM_LIGHTS, m_mainAllocator),
    m_camera(1280, 720),
    log_AppRenderer(Log("AppRenderer")) {
}

void AppRenderer::LoadAssets() {
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

  m_mainPool = &pool;

  // Pool Binds
  pool.BindSampler(m_forwardRenderer.m_sampSlot, {});
  pool.BindSampler(m_forwardRenderer.m_lightSampSlot, m_lightSamplerDesc);

  // Bind Texture
  const U32 colorTexture  = m_textureManager->Load("Meshes/sponza/color.png");
  const U32 normalTexture = m_textureManager->Load("Meshes/sponza/normal.png");

  const TextureDesc* colorDesc  = m_textureManager->GetInfo(colorTexture);
  const TextureDesc* normalDesc = m_textureManager->GetInfo(normalTexture);

  VERIFY_TRUE(log_AppRenderer, colorDesc != nullptr, "Color Texture Description was Null");
  VERIFY_TRUE(log_AppRenderer, normalDesc != nullptr, "Normal Texture Description was Null");
  pool.BindTextureData(m_forwardRenderer.m_texSlot, *colorDesc, m_textureManager->GetData(colorTexture));
  pool.BindTextureData(m_forwardRenderer.m_normalSlot, *normalDesc, m_textureManager->GetData(normalTexture));

  // Bind Light Texture
  pool.BindTextureData(m_forwardRenderer.m_lightTexSlot, m_lightTexture.GetTextureDesc(), m_lightTexture.GetBuffer());

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
  m_sponzaId = pool.CreateDrawable(createInfo);
  pool.BindVertexData(m_sponzaId, VERTEX_SLOT, vertexStart, vertexDataSize);
  pool.BindVertexData(m_sponzaId, NORMAL_SLOT, normalStart, normalDataSize);
  pool.BindVertexData(m_sponzaId, UV_SLOT, uvStart, uvDataSize);
  pool.SetIndexData(m_sponzaId, indexBufferStart, indexDataSize);
  pool.BindUniformData(m_sponzaId, m_forwardRenderer.m_uboSlot, uboDataBuffer, sizeof(UniformBufferData));
}

void AppRenderer::LoadLightTexture() {
  std::random_device rd;
  std::mt19937 mt(rd());

  const std::uniform_real_distribution<float> uniformX(LIGHT_MIN[0], LIGHT_MAX[0]);
  const std::uniform_real_distribution<float> uniformY(LIGHT_MIN[1], LIGHT_MAX[1]);
  const std::uniform_real_distribution<float> uniformZ(LIGHT_MIN[2], LIGHT_MAX[2]);

  const std::uniform_real_distribution<float> color(-0.5f, 0.5f);

  for (U32 idx = 0; idx < NUM_LIGHTS; ++idx) {

    PointLight light = {};
    light.m_position = { uniformX(mt), uniformY(mt), uniformZ(mt) };
    light.m_color = { 0.5f, 0.5f, 0.5f };
    light.m_color += Vector3f(color(mt), color(mt), color(mt));
    light.m_radius = LIGHT_RADIUS;

    LOG_DBG(log_AppRenderer, LOG_LEVEL, "Generating Light:", light.m_position[0], light.m_position[1], light.m_position[2]);
    LOG_DBG(log_AppRenderer, LOG_LEVEL, "Pos: %f, %f, %f", light.m_position[0], light.m_position[1], light.m_position[2]);
    LOG_DBG(log_AppRenderer, LOG_LEVEL, "Col: %f, %f, %f", light.m_color[0], light.m_color[1], light.m_color[2]);

    m_lights.PushBack(light);
  }

  m_lightTexture.Fill(m_lights);
}

void AppRenderer::Initialize() {

  LOG_INF(log_AppRenderer, LOG_LEVEL, "Starting Init of AppRenderer");

  m_lightSamplerDesc.m_filter       = TextureFilter::MinMagMipPoint;
  m_lightSamplerDesc.m_addressModeU = TextureAddressMode::Clamp;
  m_lightSamplerDesc.m_addressModeV = TextureAddressMode::Clamp;
  m_lightSamplerDesc.m_addressModeW = TextureAddressMode::Clamp;

  LoadLightTexture();

  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);
  m_window = RenderSystem::CreateApplicationWindow("TestZone", 1280, 720);
  VERIFY_TRUE(log_AppRenderer, m_window->Initialize(), "Cannot Initialize Window");

  m_window->SetCursorState(CursorState::Hidden);

  m_window->SetUpdateCallback([this](float timeDelta)
  {
    WindowUpdate(timeDelta);
  });

  m_window->SetMouseEventCallback([this](MouseEvent e)
  {
    m_camera.OnMouseEvent(e);
    m_window->ResetCursor();
  });

  m_window->SetKeyEventCallback([this](KeyEvent e)
  {
    m_camera.OnKeyEvent(e);
  });

  m_camera.SetTranslationStepSize(5.0f);
  m_camera.SetSensitivity(0.5f);

  ApplicationInfo appInfo;
  appInfo.m_name    = "TestZone";
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  m_sceneUBO               = {};
  m_sceneUBO.m_model       = Matrix4f::Identity();
  m_sceneUBO.m_model(1, 3) = -5;

  m_sceneUBO.m_viewProj          = m_camera.GetViewProjMatrix();
  m_sceneUBO.m_modelInvTranspose = m_sceneUBO.m_model.Inverse().Transpose();

  // TODO(vasumahesh1):[Q]:Allocator?
  const ApplicationRequirements applicationRequirements = {};

  TextureRequirements textureRequirements = {};
  textureRequirements.m_maxCount          = 10;
  textureRequirements.m_poolSize          = 0xF00000;

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(6, 4, allocatorTemporary);

  // SET 0
  m_forwardRenderer.m_uboSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Vertex
  });

  // SET 1
  m_forwardRenderer.m_sampSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::Sampler, ShaderStage::Pixel
  });

  m_forwardRenderer.m_lightSampSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::Sampler, ShaderStage::Pixel
  });

  // SET 2
  m_forwardRenderer.m_texSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::SampledImage, ShaderStage::Pixel
  });

  m_forwardRenderer.m_normalSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::SampledImage, ShaderStage::Pixel
  });

  m_forwardRenderer.m_lightTexSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::SampledImage, ShaderStage::Pixel
  });

  const U32 UBO_SET     = descriptorRequirements.AddSet({m_forwardRenderer.m_uboSlot});
  const U32 SAMPLER_SET = descriptorRequirements.AddSet({
    m_forwardRenderer.m_sampSlot, m_forwardRenderer.m_lightSampSlot
  });
  const U32 TEXTURE_SET = descriptorRequirements.AddSet({
    m_forwardRenderer.m_texSlot, m_forwardRenderer.m_normalSlot, m_forwardRenderer.m_lightTexSlot
  });

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
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID},        // SHADERS
    PipelinePassCreateInfo::Inputs{},                                          // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{},                                         // OUTPUT TARGETS
    PipelinePassCreateInfo::DescriptorSets{UBO_SET, SAMPLER_SET, TEXTURE_SET}, // DESCRIPTORS
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

  m_sceneInit = true;
}

void AppRenderer::WindowUpdate(float timeDelta) {
  if (!m_sceneInit)
  {
    return;
  }

  m_camera.Update(timeDelta);

  m_sceneUBO.m_viewProj = m_camera.GetViewProjMatrix();

  m_mainPool->BeginUpdates();

  const auto uboDataBuffer    = reinterpret_cast<const U8*>(&m_sceneUBO); // NOLINT
  m_mainPool->UpdateUniformData(m_sponzaId, m_forwardRenderer.m_uboSlot, uboDataBuffer, sizeof(UniformBufferData));

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
