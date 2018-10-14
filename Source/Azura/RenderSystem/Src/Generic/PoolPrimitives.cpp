#include "Generic/PoolPrimitives.h"
#include "Generic/Renderer.h"

using Azura::Containers::Vector; // NOLINT

namespace Azura {
namespace {
const U32 DEFAULT_VERTEX_SLOT = 0;

struct VertexWithUV {
  float m_pos[4];
  float m_uv[2];
};
} // namespace

DrawablePool& PoolPrimitives::AddScreenQuad(Renderer& renderer, U32 renderPass, Memory::Allocator& allocatorTemporary) {
  DrawablePoolCreateInfo quadPoolInfo = {allocatorTemporary};
  quadPoolInfo.m_byteSize             = 0x400000;
  quadPoolInfo.m_numDrawables         = 1;
  quadPoolInfo.m_drawType             = DrawType::InstancedIndexed;
  quadPoolInfo.m_cullMode             = CullMode::None;
  quadPoolInfo.m_renderPasses         = {{renderPass}, allocatorTemporary};
  quadPoolInfo.m_vertexDataSlots      = {
    {
      {DEFAULT_VERTEX_SLOT, BufferUsageRate::PerVertex}
    },
    allocatorTemporary
  };

  DrawablePool& quadPool = renderer.CreateDrawablePool(quadPoolInfo);

  const Vector<RawStorageFormat> quadStride = Vector<RawStorageFormat>({
    RawStorageFormat::R32G32B32A32_FLOAT, // Pos
    RawStorageFormat::R32G32_FLOAT        // UV
  }, allocatorTemporary);

  quadPool.AddBufferBinding(DEFAULT_VERTEX_SLOT, quadStride);

  Vector<VertexWithUV> quadVertexData = Vector<VertexWithUV>({
    VertexWithUV{{-1, -1, 0.9999f, 1}, {0, 0}},
    VertexWithUV{{1, -1, 0.9999f, 1}, {1, 0}},
    VertexWithUV{{1, 1, 0.9999f, 1}, {1, 1}},
    VertexWithUV{{-1, 1, 0.9999f, 1}, {0, 1}}
  }, allocatorTemporary);

  Vector<U32> quadIndexData = Vector<U32>({
    0, 1, 2,
    2, 3, 0
  }, allocatorTemporary);

  const auto quadBufferStart = reinterpret_cast<U8*>(quadVertexData.Data()); // NOLINT
  const auto quadIndexStart  = reinterpret_cast<U8*>(quadIndexData.Data());  // NOLINT

  // Create Drawable from Pool
  DrawableCreateInfo createInfo = {};
  createInfo.m_vertexCount      = quadVertexData.GetSize();
  createInfo.m_indexCount       = quadIndexData.GetSize();
  createInfo.m_instanceCount    = 1;
  createInfo.m_indexType        = RawStorageFormat::R32_UINT;

  const auto quadId = quadPool.CreateDrawable(createInfo);
  quadPool.BindVertexData(quadId, DEFAULT_VERTEX_SLOT, quadBufferStart,
                          quadVertexData.GetSize() * sizeof(VertexWithUV));
  quadPool.SetIndexData(quadId, quadIndexStart, quadIndexData.GetSize() * sizeof(U32));

  return quadPool;
}


} // namespace Azura
