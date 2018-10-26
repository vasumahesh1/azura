#include "ForwardPlus/ForwardPlusComputeScene.h"
#include "Memory/MonotonicAllocator.h"
#include "Memory/MemoryFactory.h"
#include "Generic/RenderSystem.h"
#include "Generic/PoolPrimitives.h"


namespace Azura {

namespace {
Plane ComputePlane(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2) {
  Plane plane;

  const Vector3f v0 = p1 - p0;
  const Vector3f v2 = p2 - p0;

  plane.m_normal = -1.0f * Vector3f::CrossProduct(v0, v2).Normalized();

  // Compute the distance to the origin using p0.
  plane.m_distanceToOrigin = Vector3f::DotProduct(plane.m_normal, p0);

  return plane;
}
} // namespace


ForwardPlusComputeScene::ForwardPlusComputeScene(Memory::Allocator& mainAllocator, Memory::Allocator& drawAllocator)
  : Scene("ForwardPlusComputeScene", mainAllocator, drawAllocator),
    m_lightTexture(NUM_LIGHTS, 7, mainAllocator),
    m_frustums(FPLUS_NUM_FRUSTRUMS, mainAllocator) {
}

void ForwardPlusComputeScene::Initialize(Window& window,
                                         const Camera& camera,
                                         const MeshObject& sceneMesh,
                                         const UniformBufferData& uboData,
                                         const SamplerDesc& lightSamplerDesc,
                                         const Containers::Vector<PointLight>& lights) {
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 0x40'0000);

  FillFrustums(window, camera);

  ApplicationInfo appInfo;
  appInfo.m_name    = "ForwardPlusComputeScene";
  appInfo.m_version = Version(1, 0, 0);

  DeviceRequirements requirements;
  requirements.m_discreteGPU   = true;
  requirements.m_float64       = false;
  requirements.m_int64         = false;
  requirements.m_transferQueue = false;

  const ApplicationRequirements applicationRequirements = {};

  TextureRequirements textureRequirements = {};
  textureRequirements.m_maxCount          = 2;
  textureRequirements.m_poolSize          = 0xF00000;

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(6, 5, allocatorTemporary);

  // SET 0
  m_pass.m_uboSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Vertex
  });

  // SET 1
  m_pass.m_sampSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::Sampler, ShaderStage::Pixel
  });

  // SET 2
  m_pass.m_texSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::SampledImage, ShaderStage::Pixel
  });

  m_pass.m_normalSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::SampledImage, ShaderStage::Pixel
  });

  // SET 0 for Compute
  m_pass.m_computeUBO = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Compute
  });

  m_pass.m_lightSampSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::Sampler, ShaderStage::Pixel
  });

  const U32 UBO_SET = descriptorRequirements.AddSet({
    m_pass.m_uboSlot
  });

  const U32 TEXTURE_SAMPLER_SET = descriptorRequirements.AddSet({
    m_pass.m_sampSlot
  });

  const U32 TEXTURE_SET = descriptorRequirements.AddSet({
    m_pass.m_texSlot, m_pass.m_normalSlot
  });

  const U32 COMPUTE_UBO_SET = descriptorRequirements.AddSet({
    m_pass.m_computeUBO
  });

  const U32 LIGHT_SAMPLER_SET = descriptorRequirements.AddSet({
    m_pass.m_lightSampSlot
  });

  ShaderRequirements shaderRequirements = ShaderRequirements(5, allocatorTemporary);
  const U32 VERTEX_SHADER_ID            = shaderRequirements.AddShader({
    ShaderStage::Vertex, "ForwardPlusComputePass1.vs", AssetLocation::Shaders
  });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Pixel, "ForwardPlusComputePass1.ps", AssetLocation::Shaders
  });

  const U32 COMPUTE_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Compute, "ForwardPlusComputePass2.cs", AssetLocation::Shaders
  });

  const U32 SHADING_VERTEX_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Vertex, "ScreenQuad.vs", AssetLocation::Shaders
  });

  const U32 SHADING_PIXEL_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Pixel, "ForwardPlusComputePass3.ps", AssetLocation::Shaders
  });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(6, 3, 1, allocatorTemporary);
  renderPassRequirements.m_maxPools             = 3;

  const U32 GBUFFER_1      = renderPassRequirements.AddTarget({RawStorageFormat::R32G32B32A32_FLOAT});
  const U32 GBUFFER_2      = renderPassRequirements.AddTarget({RawStorageFormat::R32G32B32A32_FLOAT});
  const U32 GBUFFER_3      = renderPassRequirements.AddTarget({RawStorageFormat::R32G32B32A32_FLOAT});
  const U32 DEPTH_BUFFER   = renderPassRequirements.AddTarget({RawStorageFormat::D32_FLOAT});
  const U32 LIGHT_TARGET   = renderPassRequirements.AddTarget({RawStorageFormat::R32G32B32A32_FLOAT, NUM_LIGHTS, 2});
  const U32 CLUSTER_TARGET = renderPassRequirements.AddTarget({
    RawStorageFormat::R32G32B32A32_FLOAT, TILES_X * TILES_Y, CLUSTER_PIXEL_HEIGHT
  });

  const U32 FRUSTUM_BUFFER = renderPassRequirements.AddBuffer({
    m_frustums.GetSize() * U32(sizeof(Frustum)), U32(sizeof(Frustum))
  });

  m_pass.m_gBufferPassId = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID},
    PipelinePassCreateInfo::InputTargets{},
    PipelinePassCreateInfo::InputBuffers{},
    PipelinePassCreateInfo::Outputs{GBUFFER_1, GBUFFER_2, GBUFFER_3, DEPTH_BUFFER},
    PipelinePassCreateInfo::DescriptorSets{UBO_SET, TEXTURE_SAMPLER_SET, TEXTURE_SET},
    ClearData{{0.0f, 0.0f, 0.0f, 1.0f}, 1.0f, 0}
  });

  m_pass.m_computePassId = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{COMPUTE_SHADER_ID},
    PipelinePassCreateInfo::InputTargets{{DEPTH_BUFFER, ShaderStage::Compute}},
    PipelinePassCreateInfo::InputBuffers{{FRUSTUM_BUFFER, ShaderStage::Compute}},
    PipelinePassCreateInfo::Outputs{LIGHT_TARGET, CLUSTER_TARGET},
    PipelinePassCreateInfo::DescriptorSets{COMPUTE_UBO_SET},
    ClearData{{0.0f, 0.0f, 0.0f, 1.0f}, 1.0f, 0},
    BlendState{},
    RenderPassType::Compute
  });

  m_pass.m_shadingPassId = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{SHADING_VERTEX_SHADER_ID, SHADING_PIXEL_SHADER_ID},
    PipelinePassCreateInfo::InputTargets{
      {GBUFFER_1, ShaderStage::Pixel}, {GBUFFER_2, ShaderStage::Pixel}, {GBUFFER_3, ShaderStage::Pixel},
      {LIGHT_TARGET, ShaderStage::Pixel}, {CLUSTER_TARGET, ShaderStage::Pixel}
    },
    PipelinePassCreateInfo::InputBuffers{},
    PipelinePassCreateInfo::Outputs{},
    PipelinePassCreateInfo::DescriptorSets{TEXTURE_SAMPLER_SET, LIGHT_SAMPLER_SET},
    ClearData{{0.0f, 0.0f, 0.0f, 1.0f}, 1.0f, 0}
  });

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            window.GetSwapChainRequirements(), renderPassRequirements,
                                            descriptorRequirements, shaderRequirements, m_mainAllocator,
                                            m_drawAllocator,
                                            window);

  m_textureManager = RenderSystem::CreateTextureManager(textureRequirements);

  // Load Assets
  m_lightTexture.Fill(lights);
  m_renderer->BindRenderTarget(LIGHT_TARGET, m_lightTexture.GetTextureDesc(), m_lightTexture.GetBuffer());

  const auto frustumDataStart = reinterpret_cast<const U8*>(m_frustums.Data()); // NOLINT
  m_renderer->BindBufferTarget(FRUSTUM_BUFFER, frustumDataStart);

  ComputePoolCreateInfo computePoolInfo = {allocatorTemporary};
  computePoolInfo.m_byteSize            = 0xF00000;
  computePoolInfo.m_computePasses       = {{m_pass.m_computePassId}, allocatorTemporary};
  computePoolInfo.m_launchDims          = ThreadGroupDimensions{TILES_X, TILES_Y, 1};

  ComputePool& computePool = m_renderer->CreateComputePool(computePoolInfo);
  m_computePool            = &computePool;

  m_lightUBO.m_nearPlane = camera.GetNearClip();
  m_lightUBO.m_farPlane  = camera.GetFarClip();
  m_lightUBO.m_view      = camera.GetViewMatrix();
  m_lightUBO.m_invProj   = camera.GetProjMatrix().Inverse();

  const auto lightUBOStart = reinterpret_cast<const U8*>(&m_lightUBO); // NOLINT
  computePool.BindUniformData(m_pass.m_computeUBO, lightUBOStart, sizeof(LightUBO));

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = 0xF00000;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_renderPasses         = {{m_pass.m_gBufferPassId}, allocatorTemporary};
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
  m_mainPool         = &pool;

  // Pool Binds
  pool.BindSampler(m_pass.m_sampSlot, {});

  // Bind Texture
  const U32 colorTexture  = m_textureManager->Load("Meshes/sponza/color.png");
  const U32 normalTexture = m_textureManager->Load("Meshes/sponza/normal.png");

  const TextureDesc* colorDesc  = m_textureManager->GetInfo(colorTexture);
  const TextureDesc* normalDesc = m_textureManager->GetInfo(normalTexture);

  VERIFY_TRUE(log_Scene, colorDesc != nullptr, "Color Texture Description was Null");
  VERIFY_TRUE(log_Scene, normalDesc != nullptr, "Normal Texture Description was Null");
  pool.BindTextureData(m_pass.m_texSlot, *colorDesc, m_textureManager->GetData(colorTexture));
  pool.BindTextureData(m_pass.m_normalSlot, *normalDesc, m_textureManager->GetData(normalTexture));

  // Bind Light Texture

  const auto vertexStart      = reinterpret_cast<const U8*>(sceneMesh.m_vertexData.data()); // NOLINT
  const auto normalStart      = reinterpret_cast<const U8*>(sceneMesh.m_normalData.data()); // NOLINT
  const auto uvStart          = reinterpret_cast<const U8*>(sceneMesh.m_uvData.data());     // NOLINT
  const auto indexBufferStart = reinterpret_cast<const U8*>(sceneMesh.m_indexData.data());  // NOLINT
  const auto uboDataBuffer    = reinterpret_cast<const U8*>(&uboData);                      // NOLINT

  // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = sceneMesh.m_numVertices;
  createInfo.m_indexCount       = sceneMesh.m_numIndices;
  createInfo.m_instanceCount    = 1;
  createInfo.m_indexType        = RawStorageFormat::R32_UINT;

  // Drawable Binds
  m_sponzaId = pool.CreateDrawable(createInfo);
  pool.BindVertexData(m_sponzaId, VERTEX_SLOT, vertexStart, sceneMesh.m_vertexDataSize);
  pool.BindVertexData(m_sponzaId, NORMAL_SLOT, normalStart, sceneMesh.m_normalDataSize);
  pool.BindVertexData(m_sponzaId, UV_SLOT, uvStart, sceneMesh.m_uvDataSize);
  pool.SetIndexData(m_sponzaId, indexBufferStart, sceneMesh.m_indexDataSize);
  pool.BindUniformData(m_sponzaId, m_pass.m_uboSlot, uboDataBuffer, sizeof(UniformBufferData));

  // PASS 3

  DrawablePool& shadingQuad = PoolPrimitives::AddScreenQuad(*m_renderer, m_pass.m_shadingPassId, allocatorTemporary);
  shadingQuad.BindSampler(m_pass.m_sampSlot, {});
  shadingQuad.BindSampler(m_pass.m_lightSampSlot, lightSamplerDesc);

  m_renderer->Submit();
}

void ForwardPlusComputeScene::Update(float timeDelta,
                                     const Camera& camera,
                                     const UniformBufferData& uboData,
                                     const Containers::Vector<PointLight>& lights) {

  UNUSED(lights);

  m_lightUBO.m_timeDelta = timeDelta;
  m_lightUBO.m_view      = camera.GetViewMatrix();
  m_lightUBO.m_nearPlane = camera.GetNearClip();
  m_lightUBO.m_farPlane  = camera.GetFarClip();
  m_lightUBO.m_invProj   = camera.GetProjMatrix().Inverse();

  const auto uboDataBuffer = reinterpret_cast<const U8*>(&uboData);    // NOLINT
  const auto lightUBOStart = reinterpret_cast<const U8*>(&m_lightUBO); // NOLINT

  m_computePool->BeginUpdates();
  m_computePool->UpdateUniformData(m_pass.m_computeUBO, lightUBOStart, sizeof(LightUBO));
  m_computePool->SubmitUpdates();

  m_mainPool->BeginUpdates();
  m_mainPool->UpdateUniformData(m_sponzaId, m_pass.m_uboSlot, uboDataBuffer, sizeof(UniformBufferData));
  m_mainPool->SubmitUpdates();

  m_renderer->RenderFrame();
}

void ForwardPlusComputeScene::CleanUp() {
}

void ForwardPlusComputeScene::FillFrustums(const Window& window, const Camera& camera) {

  const U32 widthPerTile  = window.GetWidth() / TILES_X;
  const U32 heightPerTile = window.GetHeight() / TILES_Y;

  const auto invProj = camera.GetProjMatrix().Inverse();

  for (U32 idx           = 0; idx < TILES_X; ++idx) {
    for (U32 idy         = 0; idy < TILES_Y; ++idy) {
      const U32 screenX1 = idx * widthPerTile;
      const U32 screenY1 = idy * heightPerTile;

      const U32 screenX2 = (idx + 1) * widthPerTile;
      const U32 screenY2 = (idy + 1) * heightPerTile;

      Vector2f tileP1 = Vector2f(float(screenX1), float(screenY1));
      Vector2f tileP2 = Vector2f(float(screenX2), float(screenY1));
      Vector2f tileP3 = Vector2f(float(screenX2), float(screenY2));
      Vector2f tileP4 = Vector2f(float(screenX1), float(screenY2));

      // To NDC
      tileP1 = tileP1 / Vector2f(float(window.GetWidth()), float(window.GetHeight()));
      tileP2 = tileP2 / Vector2f(float(window.GetWidth()), float(window.GetHeight()));
      tileP3 = tileP3 / Vector2f(float(window.GetWidth()), float(window.GetHeight()));
      tileP4 = tileP4 / Vector2f(float(window.GetWidth()), float(window.GetHeight()));

      tileP1[0] = (tileP1[0] * 2.0f) - 1.0f;
      tileP2[0] = (tileP2[0] * 2.0f) - 1.0f;
      tileP3[0] = (tileP3[0] * 2.0f) - 1.0f;
      tileP4[0] = (tileP4[0] * 2.0f) - 1.0f;

      tileP1[1] = 1.0f - (2.0f * tileP1[1]);
      tileP2[1] = 1.0f - (2.0f * tileP2[1]);
      tileP3[1] = 1.0f - (2.0f * tileP3[1]);
      tileP4[1] = 1.0f - (2.0f * tileP4[1]);

      // To View Space
      Vector4f viewP1 = Vector4f(tileP1[0], tileP1[1], 1.0f, 1.0f);
      Vector4f viewP2 = Vector4f(tileP2[0], tileP2[1], 1.0f, 1.0f);
      Vector4f viewP3 = Vector4f(tileP3[0], tileP3[1], 1.0f, 1.0f);
      Vector4f viewP4 = Vector4f(tileP4[0], tileP4[1], 1.0f, 1.0f);

      viewP1 = invProj * viewP1;
      viewP2 = invProj * viewP2;
      viewP3 = invProj * viewP3;
      viewP4 = invProj * viewP4;

      viewP1 = viewP1 / viewP1[3];
      viewP2 = viewP2 / viewP2[3];
      viewP3 = viewP3 / viewP3[3];
      viewP4 = viewP4 / viewP4[3];

      // viewP1.w = 1.0f; // NOLINT
      // viewP2.w = 1.0f; // NOLINT
      // viewP3.w = 1.0f; // NOLINT
      // viewP4.w = 1.0f; // NOLINT

      Frustum frustum;
      frustum.m_planes[0] = ComputePlane(Vector3f(0,0,0), viewP1.xyz(), viewP2.xyz());
      frustum.m_planes[1] = ComputePlane(Vector3f(0,0,0), viewP2.xyz(), viewP3.xyz());
      frustum.m_planes[2] = ComputePlane(Vector3f(0,0,0), viewP3.xyz(), viewP4.xyz());
      frustum.m_planes[3] = ComputePlane(Vector3f(0,0,0), viewP4.xyz(), viewP1.xyz());

      m_frustums.PushBack(frustum);
    }
  }
}
} // namespace Azura
