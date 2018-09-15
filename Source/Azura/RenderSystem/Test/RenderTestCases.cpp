#include "RenderTestCases.h"
#include "Generic/RenderSystem.h"
#include "Generic/Shader.h"
#include "Memory/MonotonicAllocator.h"
#include "Memory/MemoryFactory.h"
#include "Math/Transform.h"

using namespace Azura;             // NOLINT
using namespace Azura::Containers; // NOLINT

struct Vertex {
  float m_pos[4];
  float m_col[4];
};

void RenderTestCases::ExecuteBasicRenderTest(Azura::Renderer& renderer,
                                             Azura::Window& window,
                                             const Azura::Log& log_TestCase) {
  STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 1024);
  UNUSED(window);

  renderer.SetDrawablePoolCount(1);

  auto vertShader = RenderSystem::CreateShader(renderer,
                                               "Shaders/" + renderer.GetRenderingAPI() + "/RenderSystemTest.vertex",
                                               log_TestCase);
  vertShader->SetStage(ShaderStage::Vertex);

  auto pixelShader = RenderSystem::CreateShader(renderer,
                                                "Shaders/" + renderer.GetRenderingAPI() + "/RenderSystemTest.pixel",
                                                log_TestCase);
  pixelShader->SetStage(ShaderStage::Pixel);

  DrawablePoolCreateInfo poolInfo = {};
  poolInfo.m_byteSize             = 4096;
  poolInfo.m_numDrawables         = 1;
  poolInfo.m_numShaders           = 2;
  DrawablePool& pool              = renderer.CreateDrawablePool(poolInfo);

  pool.AddShader(*vertShader);
  pool.AddShader(*pixelShader);

  Slot vertexDataSlot      = {};
  vertexDataSlot.m_binding = 0;
  vertexDataSlot.m_rate    = BufferUsageRate::PerVertex;

  Vector<RawStorageFormat> vertexStride = Vector<RawStorageFormat>(ContainerExtent{2, 2}, allocatorTemporary);
  vertexStride[0]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  vertexStride[1]                       = RawStorageFormat::R32G32B32A32_FLOAT;
  pool.AddBufferBinding(vertexDataSlot, vertexStride);

  Vector<Vertex> vertexData = Vector<Vertex>({
    Vertex{{0, 0, 1, 1}, {1, 0, 0, 1}},
    Vertex{{1, 0, 1, 1}, {0, 1, 0, 1}},
    Vertex{{1, 1, 1, 1}, {0, 0, 1, 1}},
    Vertex{{0, 1, 1, 1}, {1, 1, 1, 1}}
  }, allocatorTemporary);

  Vector<U32> indexData = Vector<U32>({
    0, 2, 1,
    2, 0, 3
  }, allocatorTemporary);

  UniformBufferData uboData = {};
  uboData.m_model = Matrix4f::Identity();
  uboData.m_view = Transform::LookAt(Vector3f(0.5f, 0.5f, 0.0f), Vector3f(0.5f, 0.5f, -6.0f), Vector3f(0.0f, 1.0f, 0.0f));
  uboData.m_proj = Transform::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

  const auto bufferStart      = reinterpret_cast<U8*>(vertexData.Data()); // NOLINT
  const auto indexBufferStart = reinterpret_cast<U8*>(indexData.Data());  // NOLINT
  const auto uboDataBuffer    = reinterpret_cast<U8*>(&uboData);          // NOLINT
  // Create Drawable from Pool
  Drawable& drawable = pool.CreateDrawable();
  drawable.SetDrawMode(DrawType::InstancedIndexed);
  drawable.SetIndexCount(indexData.GetSize());
  drawable.SetVertexCount(vertexData.GetSize());
  drawable.SetInstanceCount(1);
  drawable.SetUniformCount(1);
  drawable.SetIndexFormat(RawStorageFormat::R32_UNORM);

  drawable.SetInstanceDataCount(0);
  drawable.SetVertexDataCount(1);
  drawable.SetUniformDataCount(1);

  drawable.AddVertexData(vertexDataSlot, bufferStart, vertexData.GetSize() * sizeof(Vertex));
  drawable.SetIndexData(indexBufferStart, indexData.GetSize() * sizeof(U32));
  drawable.AddUniformData(uboDataBuffer, sizeof(UniformBufferData), 0);

  // All Drawables Done
  renderer.Submit();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.RenderFrame();
  renderer.SnapshotFrame("./BasicRenderTest.data");
}
