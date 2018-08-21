#pragma once
#include "Types.h"
#include "GenericTypes.h"
#include "Containers/Vector.h"

namespace Azura {

struct DataBuffer {
  explicit DataBuffer(Memory::Allocator& allocator);

  BufferInfo m_info;
  Containers::Vector<U8> m_data;
};

struct DrawableCreateInfo {

  explicit DrawableCreateInfo(Memory::Allocator& allocator);

  DrawType m_drawType{DrawType::InstancedIndexed};
  U32 m_instances{0};
  U32 m_vertexCount{0};
  U32 m_indexCount{0};

  U32 m_shader{0};
  Containers::Vector<DataBuffer> m_buffers;
};

class Drawable {

public:
  Drawable(U32 vertexCount, U32 indexCount, Memory::Allocator& allocator);
  virtual ~Drawable() = default;

  Drawable(const Drawable& other) = delete;
  Drawable(Drawable&& other) noexcept = default;
  Drawable& operator=(const Drawable& other) = delete;
  Drawable& operator=(Drawable&& other) noexcept = default;

protected:
  U32 VertexCount() const;
  U32 IndexCount() const;

private:
  U32 m_vertexCount;
  U32 m_indexCount;
  Containers::Vector<BufferInfo> m_bufferInfos;
};

struct DrawablePoolCreateInfo {
  U32 m_byteSize;
};

class DrawablePool {
public:
  explicit DrawablePool(U32 byteSize);
  virtual ~DrawablePool() = default;

  DrawablePool(const DrawablePool& other) = delete;
  DrawablePool(DrawablePool&& other) noexcept = default;
  DrawablePool& operator=(const DrawablePool& other) = delete;
  DrawablePool& operator=(DrawablePool&& other) noexcept = default;

  virtual U32 CreateDrawable(const DrawableCreateInfo& createInfo) = 0;
  virtual Drawable& GetDrawable(U32 id) = 0;

protected:
  U32 Size() const;

private:
  U32 m_byteSize;
};

} // namespace Azura
