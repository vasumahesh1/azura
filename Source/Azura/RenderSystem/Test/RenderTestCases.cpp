#include "RenderTestCases.h"
#include "Generic/RenderSystem.h"
#include "Generic/Shader.h"
#include "Memory/MonotonicAllocator.h"
#include "Memory/MemoryFactory.h"
#include "Math/Transform.h"

using namespace Azura;             // NOLINT
using namespace Azura::Containers; // NOLINT

#define VERTEX_SLOT 0
#define INSTANCE_SLOT 1

struct Vertex {
  float m_pos[4];
  float m_col[4];
};

struct VertexWithUV {
  float m_pos[4];
  float m_uv[2];
};

struct Instance {
  float m_pos[4];
};

void RenderTestCases::ExecuteBasicRenderTest(Azura::Renderer& renderer,
                                             Azura::Window& window,
                                             U32 renderPass,
                                             U32 uboSlot,
                                             const Azura::Log& log_TestCase) {
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);
  UNUSED(log_TestCase);
  UNUSED(window);

  renderer.SetDrawablePoolCount(1);

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = 0x400000;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_drawType             = DrawType::InstancedIndexed;
  poolInfo.m_renderPasses = {{renderPass}, allocatorTemporary};
  poolInfo.m_vertexDataSlots      = {
    {
      {VERTEX_SLOT, BufferUsageRate::PerVertex}
    },
    allocatorTemporary
  };

  DrawablePool& pool = renderer.CreateDrawablePool(poolInfo);

  Vector<RawStorageFormat> vertexStride = Vector<RawStorageFormat>(ContainerExtent{2, 2}, allocatorTemporary);
  vertexStride[0]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  vertexStride[1]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  pool.AddBufferBinding(VERTEX_SLOT, vertexStride);

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

  UniformBufferData uboData = {};
  uboData.m_model           = Matrix4f::Identity();
  const auto view           = Transform::LookAt(Vector3f(0.5f, 0.5f, 0.0f), Vector3f(0.5f, 0.5f, 6.0f),
                                                Vector3f(0.0f, 1.0f, 0.0f));
  const auto proj = Transform::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

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
  pool.BindUniformData(drawableId, uboSlot, uboDataBuffer, sizeof(UniformBufferData));

  // All Drawables Done
  renderer.Submit();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.SnapshotFrame("./BasicRenderTest.data");
}

void RenderTestCases::ExecuteBasicInstancingTest(Azura::Renderer& renderer,
                                                 Azura::Window& window,
                                                 Azura::U32 renderPass,
                                                 U32 uboSlot,
                                                 const Azura::Log& log_TestCase) {
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);
  UNUSED(window);
  UNUSED(log_TestCase);

  renderer.SetDrawablePoolCount(1);

  DrawablePoolCreateInfo poolInfo{allocatorTemporary};
  poolInfo.m_byteSize        = 0x400000;
  poolInfo.m_numDrawables    = 1;
  poolInfo.m_renderPasses = {{renderPass}, allocatorTemporary};
  poolInfo.m_vertexDataSlots = {
    {
      {VERTEX_SLOT, BufferUsageRate::PerVertex},
      {INSTANCE_SLOT, BufferUsageRate::PerInstance}
    },
    allocatorTemporary
  };

  DrawablePool& pool = renderer.CreateDrawablePool(poolInfo);

  Vector<RawStorageFormat> vertexStride = Vector<RawStorageFormat>(ContainerExtent{2}, allocatorTemporary);
  vertexStride[0]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  vertexStride[1]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  pool.AddBufferBinding(VERTEX_SLOT, vertexStride);

  Vector<RawStorageFormat> instanceStride = Vector<RawStorageFormat>(ContainerExtent{1}, allocatorTemporary);
  instanceStride[0]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  pool.AddBufferBinding(INSTANCE_SLOT, instanceStride);

  Vector<Vertex> vertexData = Vector<Vertex>({
    Vertex{{0, 0, 1, 1}, {1, 0, 0, 1}},
    Vertex{{1, 0, 1, 1}, {0, 1, 0, 1}},
    Vertex{{1, 1, 1, 1}, {0, 0, 1, 1}},
    Vertex{{0, 1, 1, 1}, {1, 1, 1, 1}}
  }, allocatorTemporary);

  Vector<Instance> instanceData = Vector<Instance>({
    Instance{{-2, 0, 0, 1}},
    Instance{{2, 0, 0, 1}}
  }, allocatorTemporary);

  Vector<U32> indexData = Vector<U32>({
    0, 1, 2,
    2, 3, 0
  }, allocatorTemporary);

  UniformBufferData uboData = {};
  uboData.m_model           = Matrix4f::Identity();
  const auto view           = Transform::LookAt(Vector3f(0.5f, 0.5f, 0.0f), Vector3f(0.5f, 0.5f, 6.0f),
                                                Vector3f(0.0f, 1.0f, 0.0f));
  const auto proj = Transform::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

  uboData.m_viewProj          = proj * view;
  uboData.m_modelInvTranspose = uboData.m_model.Inverse().Transpose();

  const auto bufferStart      = reinterpret_cast<U8*>(vertexData.Data());   // NOLINT
  const auto instanceStart    = reinterpret_cast<U8*>(instanceData.Data()); // NOLINT
  const auto indexBufferStart = reinterpret_cast<U8*>(indexData.Data());    // NOLINT
  const auto uboDataBuffer    = reinterpret_cast<U8*>(&uboData);            // NOLINT
  // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = vertexData.GetSize();
  createInfo.m_indexCount       = indexData.GetSize();
  createInfo.m_instanceCount    = 2;
  createInfo.m_indexType        = RawStorageFormat::R32_UINT;

  const auto drawableId = pool.CreateDrawable(createInfo);
  pool.BindVertexData(drawableId, VERTEX_SLOT, bufferStart, vertexData.GetSize() * sizeof(Vertex));
  pool.BindInstanceData(drawableId, INSTANCE_SLOT, instanceStart, instanceData.GetSize() * sizeof(Instance));
  pool.SetIndexData(drawableId, indexBufferStart, indexData.GetSize() * sizeof(U32));
  pool.BindUniformData(drawableId, uboSlot, uboDataBuffer, sizeof(UniformBufferData));

  // All Drawables Done
  renderer.Submit();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.SnapshotFrame("./BasicInstancingTest.data");
}

void RenderTestCases::ExecuteBasicTextureTest(Azura::Renderer& renderer,
                                              Azura::Window& window,
                                              Azura::U32 renderPass,
                                              Azura::U32 uboSlot,
                                              Azura::U32 samplerSlot,
                                              Azura::U32 samplerTextureSlot,
                                              const Azura::Log& log_TestCase) {
  HEAP_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 16384);
  UNUSED(window);

  TextureRequirements textureRequirements = {};
  textureRequirements.m_maxCount          = 1;
  textureRequirements.m_poolSize          = 0x400000; // 4MB
  auto texManager                         = RenderSystem::CreateTextureManager(renderer, textureRequirements,
                                                                               log_TestCase);

  const U32 nocturnalTexture = texManager->Load("Textures/Nocturnal.jpg");

  renderer.SetDrawablePoolCount(1);

  DrawablePoolCreateInfo poolInfo = {allocatorTemporary};
  poolInfo.m_byteSize             = 0x400000;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_renderPasses = {{renderPass}, allocatorTemporary};
  poolInfo.m_drawType             = DrawType::InstancedIndexed;
  poolInfo.m_vertexDataSlots      = {
    {
      {VERTEX_SLOT, BufferUsageRate::PerVertex}
    },
    allocatorTemporary
  };


  DrawablePool& pool = renderer.CreateDrawablePool(poolInfo);

  Vector<RawStorageFormat> vertexStride = Vector<RawStorageFormat>(ContainerExtent{2, 2}, allocatorTemporary);
  vertexStride[0]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  vertexStride[1]                       = RawStorageFormat::R32G32_FLOAT;
  pool.AddBufferBinding(VERTEX_SLOT, vertexStride);

  const TextureDesc* desc = texManager->GetInfo(nocturnalTexture);
  VERIFY_TRUE(log_TestCase, desc != nullptr, "Texture Description was Null");

  pool.BindTextureData(samplerTextureSlot, *desc, texManager->GetData(nocturnalTexture));
  pool.BindSampler(samplerSlot, {});

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

  UniformBufferData uboData = {};
  uboData.m_model           = Matrix4f::Identity();
  const Matrix4f view       = Transform::LookAt(Vector3f(0.5f, 0.5f, 1.0f), Vector3f(0.5f, 0.5f, 4.0f),
                                                Vector3f(0.0f, 1.0f, 0.0f));
  const Matrix4f proj = Transform::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

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
  pool.BindVertexData(drawableId, VERTEX_SLOT, bufferStart, vertexData.GetSize() * sizeof(VertexWithUV));
  pool.SetIndexData(drawableId, indexBufferStart, indexData.GetSize() * sizeof(U32));
  pool.BindUniformData(drawableId, uboSlot, uboDataBuffer, sizeof(UniformBufferData));

  // All Drawables Done
  renderer.Submit();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.SnapshotFrame("./BasicTextureTest.data");

}
