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

#define VERTEX_SLOT 0
#define NORMAL_SLOT 1

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
  Color4f m_waterColor{21.0, 92.0, 158.0, 1.0f};
};

AppRenderer::AppRenderer()
  : m_mainBuffer(16384),
    m_mainAllocator(m_mainBuffer, 8192),
    m_drawableAllocator(m_mainBuffer, 8192),
    m_camera(1280, 720),
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
  ApplicationRequirements applicationRequirements = {};
  applicationRequirements.m_clearColor[0]         = 0.2f;
  applicationRequirements.m_clearColor[1]         = 0.2f;
  applicationRequirements.m_clearColor[2]         = 0.2f;

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(4, allocatorTemporary);
  // Set 0
  const U32 UBO_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex | ShaderStage::Pixel });
  
  // Set 1
  const U32 SHADER_CONTROLS_SLOT = descriptorRequirements.AddDescriptor({ DescriptorType::UniformBuffer, ShaderStage::Vertex | ShaderStage::Pixel });

  // Set 2
  const U32 SAMPLER_SLOT = descriptorRequirements.AddDescriptor({DescriptorType::Sampler, ShaderStage::Pixel});
  const U32 PLANET_TEXTURE_SLOT = descriptorRequirements.AddDescriptor({DescriptorType::SampledImage, ShaderStage::Pixel, DescriptorBinding::Same});


  ShaderRequirements shaderRequirements = ShaderRequirements(4, allocatorTemporary);
  const U32 VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "Terrain.vs", AssetLocation::Shaders });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "Terrain.ps", AssetLocation::Shaders });

  const U32 SKY_VERTEX_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Vertex, "Sky.vs", AssetLocation::Shaders });
  const U32 SKY_PIXEL_SHADER_ID = shaderRequirements.AddShader({ ShaderStage::Pixel, "Sky.ps", AssetLocation::Shaders });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(0, 1, allocatorTemporary);

  const U32 SINGLE_PASS = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID},  // SHADERS
    PipelinePassCreateInfo::Inputs{},                                    // INPUT TARGETS
    PipelinePassCreateInfo::Outputs{},                                   // OUTPUT TARGETS
    PipelinePassCreateInfo::Descriptors{UBO_SLOT}                        // DESCRIPTORS
    });

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
    m_window->GetSwapChainRequirements(), renderPassRequirements,
    descriptorRequirements, shaderRequirements, m_mainAllocator, m_drawableAllocator,
    *m_window);
  m_renderer->SetDrawablePoolCount(2);

  DrawablePool& screenQuad = PoolPrimitives::AddScreenQuad(*m_renderer, SINGLE_PASS, allocatorTemporary);
  screenQuad.AddShader(SKY_VERTEX_SHADER_ID);
  screenQuad.AddShader(SKY_PIXEL_SHADER_ID);
  screenQuad.BindUniformData(0, UBO_SLOT, uboDataBuffer, sizeof(UniformBufferData));
  screenQuad.BindUniformData(0, SHADER_CONTROLS_SLOT, shaderControlBuffer, sizeof(ShaderControls));

  IcoSphere sphere(8);

  DrawablePoolCreateInfo poolInfo(allocatorTemporary);
  poolInfo.m_byteSize        = sphere.TotalDataSize() + 0x400000;
  poolInfo.m_numDrawables    = 1;
  poolInfo.m_cullMode        = CullMode::FrontBit;
  poolInfo.m_drawType        = DrawType::InstancedIndexed;
  poolInfo.m_renderPasses    = {{SINGLE_PASS}, allocatorTemporary};
  poolInfo.m_vertexDataSlots = {
    {
      {VERTEX_SLOT, BufferUsageRate::PerVertex},
      {NORMAL_SLOT, BufferUsageRate::PerVertex}
    },
    allocatorTemporary
  };

  TextureRequirements textureRequirements = {};
  textureRequirements.m_maxCount = 1;
  textureRequirements.m_poolSize = 0x400000; // 4MB

  m_textureManager = RenderSystem::CreateTextureManager(*m_renderer, textureRequirements, log_AppRenderer);

  const U32 planet1Texture = m_textureManager->Load("Textures/Planet1_Texture.jpg");
  const TextureDesc* planet1Desc = m_textureManager->GetInfo(planet1Texture);
  VERIFY_TRUE(log_AppRenderer, planet1Desc != nullptr, "planet1Desc was Null");

  DrawablePool& pool = m_renderer->CreateDrawablePool(poolInfo);

  pool.BindTextureData(PLANET_TEXTURE_SLOT, *planet1Desc, m_textureManager->GetData(planet1Texture));
  pool.BindSampler(SAMPLER_SLOT, {});

  Vector<RawStorageFormat> vertexStride = Vector<RawStorageFormat>(1, allocatorTemporary);
  vertexStride.PushBack(sphere.GetVertexFormat());
  pool.AddBufferBinding(VERTEX_SLOT, vertexStride);

  pool.AddBufferBinding(NORMAL_SLOT, {{sphere.GetNormalFormat()}, allocatorTemporary});

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
