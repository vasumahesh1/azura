#include "ProceduralPlanet/AppRenderer.h"
#include "Core/RawStorageFormat.h"
#include "Generic/RenderSystem.h"
#include "Generic/Shader.h"
#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"
#include "Math/Transform.h"
#include "Math/Icosphere.h"
#include "Generic/PoolPrimitives.h"

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

struct UniformBufferData {
  Matrix4f m_model;
  Matrix4f m_modelInvTranspose;
  Matrix4f m_viewProj;
  Matrix4f m_invViewProj;
};

struct ShaderControls {
  // NOLINT
  float m_shoreLevel{0.5f};
  float m_elevation{0.5f};
  float m_noiseScale{0.5f};
  float pad1;

  Vector4f m_lightPos{0.0f, 0.0f, 15.0f, 1.0f};
  Vector4f m_eye{0.0f, 0.0f, 4.0f, 1.0f};

  Color4f m_waterControls{0.5f, 0.65f, 0, 0};
  Color4f m_waterColor{21.0f / 255.0f, 92.0f / 255.0f, 158.0f / 255.0f, 1.0f};
};

AppRenderer::AppRenderer()
  : m_mainBuffer(16384 * 2),
    m_mainAllocator(m_mainBuffer, 16384),
    m_drawableAllocator(m_mainBuffer, 16384),
    m_camera(1280, 720),
    log_AppRenderer(Log("AppRenderer")) {
}

void AppRenderer::Initialize() {
  LOG_INF(log_AppRenderer, LOG_LEVEL, "Starting Init of AppRenderer");

  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 8192);
  m_window = RenderSystem::CreateApplicationWindow("ProceduralPlanet", 1280, 720);

  m_window->SetUpdateCallback([this](float timeSinceLastFrame)
  {
    UNUSED(timeSinceLastFrame);
    WindowUpdate();
  });

  m_window->SetMouseEventCallback([this](MouseEvent e)
  {
    m_camera.OnMouseEvent(e);
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

  ShaderControls shaderControls{};

  UniformBufferData uboData = {};
  uboData.m_model           = Matrix4f::Identity();
  uboData.m_viewProj = m_camera.GetViewProjMatrix();
  uboData.m_invViewProj = m_camera.GetInvViewProjMatrix();

  uboData.m_modelInvTranspose = uboData.m_model.Inverse().Transpose();

  const auto uboDataBuffer       = reinterpret_cast<U8*>(&uboData);        // NOLINT
  const auto shaderControlBuffer = reinterpret_cast<U8*>(&shaderControls); // NOLINT

  // TODO(vasumahesh1):[Q]:Allocator?
  ApplicationRequirements applicationRequirements = {}; // NOLINT

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(4, 4, allocatorTemporary);
  const U32 UBO_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex | ShaderStage::Pixel });
  const U32 SHADER_CONTROLS_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex | ShaderStage::Pixel });
  const U32 SAMPLER_SLOT = descriptorRequirements.AddDescriptor({DescriptorType::Sampler, ShaderStage::Pixel});
  const U32 PLANET_TEXTURE_SLOT = descriptorRequirements.AddDescriptor({DescriptorType::SampledImage, ShaderStage::Pixel});

  const U32 UBO_SET = descriptorRequirements.AddSet({ UBO_SLOT });
  const U32 CONTROLS_SET = descriptorRequirements.AddSet({ SHADER_CONTROLS_SLOT });
  const U32 SAMPLER_SET = descriptorRequirements.AddSet({ SAMPLER_SLOT });
  const U32 TEXTURE_SET = descriptorRequirements.AddSet({ PLANET_TEXTURE_SLOT });

  ShaderRequirements shaderRequirements = ShaderRequirements(6, allocatorTemporary);
  const U32 SCREEN_QUAD_VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "ScreenQuad.vs", AssetLocation::Shaders });

  const U32 NOISE_VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "Noise.vs", AssetLocation::Shaders });
  const U32 NOISE_PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "Noise.ps", AssetLocation::Shaders });
  
  const U32 TERRAIN_PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "Terrain.ps", AssetLocation::Shaders });
  const U32 SKY_PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "Sky.ps", AssetLocation::Shaders });
  const U32 WATER_PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "Water.ps", AssetLocation::Shaders });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(4, 2, allocatorTemporary);
  renderPassRequirements.m_maxPools = 4;

  const U32 NOISE_TARGET_1 = renderPassRequirements.AddTarget({RawStorageFormat::R32G32B32A32_FLOAT});
  const U32 NOISE_TARGET_2 = renderPassRequirements.AddTarget({RawStorageFormat::R32G32B32A32_FLOAT});
  const U32 NOISE_TARGET_3 = renderPassRequirements.AddTarget({RawStorageFormat::R32G32B32A32_FLOAT});
  const U32 NOISE_DEPTH = renderPassRequirements.AddTarget({RawStorageFormat::D32_FLOAT});

  const U32 NOISE_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{NOISE_VERTEX_SHADER_ID, NOISE_PIXEL_SHADER_ID},  // SHADERS
    PipelinePassCreateInfo::Inputs{},                                                // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{NOISE_TARGET_1, NOISE_TARGET_2, NOISE_TARGET_3, NOISE_DEPTH} , // OUTPUT TARGETS
    PipelinePassCreateInfo::DescriptorSets{UBO_SET, CONTROLS_SET}
    });

  const U32 SINGLE_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{},                                   // SHADERS
    PipelinePassCreateInfo::Inputs{{NOISE_TARGET_1, ShaderStage::Pixel}, {NOISE_TARGET_2, ShaderStage::Pixel}, {NOISE_TARGET_3, ShaderStage::Pixel}},      // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{},                                   // OUTPUT TARGETS
    PipelinePassCreateInfo::DescriptorSets{UBO_SET, CONTROLS_SET, SAMPLER_SET, TEXTURE_SET},
    {},
    BlendState{true, {BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha}, {BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha}}
    });

  SwapChainRequirements swapChainRequirements = m_window->GetSwapChainRequirements();
  swapChainRequirements.m_depthFormat = RawStorageFormat::UNKNOWN;

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
    swapChainRequirements, renderPassRequirements,
    descriptorRequirements, shaderRequirements, m_mainAllocator, m_drawableAllocator,
    *m_window);

  IcoSphere sphere(8);

  DrawablePoolCreateInfo poolInfo(allocatorTemporary);
  poolInfo.m_byteSize        = sphere.TotalDataSize() + 0x400000;
  poolInfo.m_numDrawables    = 1;
  poolInfo.m_cullMode        = CullMode::BackBit;
  poolInfo.m_drawType        = DrawType::InstancedIndexed;
  poolInfo.m_renderPasses    = {{NOISE_PASS}, allocatorTemporary};

  const auto VERTEX_SLOT = poolInfo.AddInputSlot({ BufferUsageRate::PerVertex, { {"POSITION", sphere.GetVertexFormat()}} });
  const auto NORMAL_SLOT = poolInfo.AddInputSlot({ BufferUsageRate::PerVertex, { {"NORMAL", sphere.GetNormalFormat()}} });

  TextureRequirements textureRequirements = {};
  textureRequirements.m_maxCount = 1;
  textureRequirements.m_poolSize = 0x400000; // 4MB

  m_textureManager = RenderSystem::CreateTextureManager(textureRequirements);

  const U32 planet1Texture = m_textureManager->Load("Textures/Planet1_Texture.jpg");
  const TextureDesc* planet1Desc = m_textureManager->GetInfo(planet1Texture);
  VERIFY_TRUE(log_AppRenderer, planet1Desc != nullptr, "planet1Desc was Null");

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = sphere.GetVertexCount();
  createInfo.m_indexCount       = sphere.GetIndexCount();
  createInfo.m_instanceCount    = 1;
  createInfo.m_indexType        = sphere.GetIndexFormat();

  const auto drawableId = pool.CreateDrawable(createInfo);
  pool.BindVertexData(drawableId, VERTEX_SLOT, sphere.VertexData(), sphere.VertexDataSize());
  pool.BindVertexData(drawableId, NORMAL_SLOT, sphere.NormalData(), sphere.NormalDataSize());
  pool.SetIndexData(drawableId, sphere.IndexData(), sphere.IndexDataSize());
  pool.BindUniformData(drawableId, UBO_SLOT, uboDataBuffer, sizeof(UniformBufferData));
  pool.BindUniformData(drawableId, SHADER_CONTROLS_SLOT, shaderControlBuffer, sizeof(ShaderControls));

  DrawablePool& skyQuad = PoolPrimitives::AddScreenQuad(*m_renderer, SINGLE_PASS, allocatorTemporary);
  skyQuad.AddShader(SCREEN_QUAD_VERTEX_SHADER_ID);
  skyQuad.AddShader(SKY_PIXEL_SHADER_ID);
  skyQuad.BindUniformData(0, UBO_SLOT, uboDataBuffer, sizeof(UniformBufferData));
  skyQuad.BindUniformData(0, SHADER_CONTROLS_SLOT, shaderControlBuffer, sizeof(ShaderControls));
  skyQuad.BindSampler(SAMPLER_SLOT, {});

  DrawablePool& terrainQuad = PoolPrimitives::AddScreenQuad(*m_renderer, SINGLE_PASS, allocatorTemporary);
  terrainQuad.AddShader(SCREEN_QUAD_VERTEX_SHADER_ID);
  terrainQuad.AddShader(TERRAIN_PIXEL_SHADER_ID);
  terrainQuad.BindUniformData(0, UBO_SLOT, uboDataBuffer, sizeof(UniformBufferData));
  terrainQuad.BindUniformData(0, SHADER_CONTROLS_SLOT, shaderControlBuffer, sizeof(ShaderControls));
  terrainQuad.BindTextureData(PLANET_TEXTURE_SLOT, *planet1Desc, m_textureManager->GetData(planet1Texture));
  terrainQuad.BindSampler(SAMPLER_SLOT, {});

  DrawablePool& waterQuad = PoolPrimitives::AddScreenQuad(*m_renderer, SINGLE_PASS, allocatorTemporary);
  waterQuad.AddShader(SCREEN_QUAD_VERTEX_SHADER_ID);
  waterQuad.AddShader(WATER_PIXEL_SHADER_ID);
  waterQuad.BindUniformData(0, UBO_SLOT, uboDataBuffer, sizeof(UniformBufferData));
  waterQuad.BindUniformData(0, SHADER_CONTROLS_SLOT, shaderControlBuffer, sizeof(ShaderControls));
  waterQuad.BindTextureData(PLANET_TEXTURE_SLOT, *planet1Desc, m_textureManager->GetData(planet1Texture));
  waterQuad.BindSampler(SAMPLER_SLOT, {});

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
