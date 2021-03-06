#include "Forward/ForwardScene.h"
#include "Memory/MonotonicAllocator.h"
#include "Memory/MemoryFactory.h"
#include "Generic/RenderSystem.h"


namespace Azura {


ForwardScene::ForwardScene(Memory::Allocator& mainAllocator, Memory::Allocator& drawAllocator)
  : Scene("ForwardScene", mainAllocator, drawAllocator),
    m_lightTexture(NUM_LIGHTS, 7, mainAllocator) {
}

void ForwardScene::Initialize(Window& window,
                              const Camera& camera,
                              const MeshObject& sceneMesh,
                              const UniformBufferData& uboData,
                              const SamplerDesc& lightSamplerDesc,
                              const Containers::Vector<PointLight>& lights) {
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 0x40'0000);

  UNUSED(camera);

  ApplicationInfo appInfo;
  appInfo.m_name    = "ForwardRenderer";
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

  DescriptorRequirements descriptorRequirements = DescriptorRequirements(6, 4, allocatorTemporary);

  // SET 0
  m_pass.m_uboSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::UniformBuffer, ShaderStage::Vertex
  });

  // SET 1
  m_pass.m_sampSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::Sampler, ShaderStage::Pixel
  });

  m_pass.m_lightSampSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::Sampler, ShaderStage::Pixel
  });

  // SET 2
  m_pass.m_texSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::SampledImage, ShaderStage::Pixel
  });

  m_pass.m_normalSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::SampledImage, ShaderStage::Pixel
  });

  m_pass.m_lightTexSlot = descriptorRequirements.AddDescriptor({
    DescriptorType::SampledImage, ShaderStage::Pixel
  });

  const U32 UBO_SET     = descriptorRequirements.AddSet({m_pass.m_uboSlot});
  const U32 SAMPLER_SET = descriptorRequirements.AddSet({
    m_pass.m_sampSlot, m_pass.m_lightSampSlot
  });
  const U32 TEXTURE_SET = descriptorRequirements.AddSet({
    m_pass.m_texSlot, m_pass.m_normalSlot, m_pass.m_lightTexSlot
  });

  ShaderRequirements shaderRequirements = ShaderRequirements(2, allocatorTemporary);
  const U32 VERTEX_SHADER_ID            = shaderRequirements.AddShader({
    ShaderStage::Vertex, "Forward.vs", AssetLocation::Shaders
  });
  const U32 PIXEL_SHADER_ID = shaderRequirements.AddShader({
    ShaderStage::Pixel, "Forward.ps", AssetLocation::Shaders
  });

  RenderPassRequirements renderPassRequirements = RenderPassRequirements(1, 2, 0, allocatorTemporary);
  renderPassRequirements.m_maxPools             = 1;

  m_pass.m_passId = renderPassRequirements.AddPass({
    PipelinePassCreateInfo::Shaders{VERTEX_SHADER_ID, PIXEL_SHADER_ID},        // SHADERS
    PipelinePassCreateInfo::InputTargets{},                                          // INPUT TARGETS
    PipelinePassCreateInfo::InputBuffers{},
    PipelinePassCreateInfo::OutputTargets{},                                         // OUTPUT TARGETS
    PipelinePassCreateInfo::OutputBuffers{},                                         // OUTPUT TARGETS
    PipelinePassCreateInfo::DescriptorSets{UBO_SET, SAMPLER_SET, TEXTURE_SET}, // DESCRIPTORS
    ClearData{{0.2f, 0.2f, 0.2f, 1.0f}, 1.0f, 0}
  });

  m_renderer = RenderSystem::CreateRenderer(appInfo, requirements, applicationRequirements,
                                            window.GetSwapChainRequirements(), renderPassRequirements,
                                            descriptorRequirements, shaderRequirements, m_mainAllocator,
                                            m_drawAllocator,
                                            window);

  m_textureManager = RenderSystem::CreateTextureManager(textureRequirements);

  // Load Assets
  m_lightTexture.Fill(lights);

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = 0xF00000;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_renderPasses         = {{m_pass.m_passId}, allocatorTemporary};
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
  pool.BindSampler(m_pass.m_lightSampSlot, lightSamplerDesc);

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
  pool.BindTextureData(m_pass.m_lightTexSlot, m_lightTexture.GetTextureDesc(), m_lightTexture.GetBuffer());

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

  m_renderer->Submit();
}

void ForwardScene::Update(float timeDelta,
                          const Camera& camera,
                          const UniformBufferData& uboData,
                          const Containers::Vector<PointLight>& lights) {

  UNUSED(timeDelta);
  UNUSED(camera);

  m_lightTexture.Fill(lights);

  m_mainPool->BeginUpdates();
  const auto uboDataBuffer = reinterpret_cast<const U8*>(&uboData); // NOLINT
  m_mainPool->UpdateUniformData(m_sponzaId, m_pass.m_uboSlot, uboDataBuffer, sizeof(UniformBufferData));
  m_mainPool->UpdateTextureData(m_pass.m_lightTexSlot, m_lightTexture.GetBuffer());
  m_mainPool->SubmitUpdates();

  m_renderer->RenderFrame();
}

void ForwardScene::CleanUp() {
}
} // namespace Azura
