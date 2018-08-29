#pragma once
#include "Types.h"
#include "GenericTypes.h"
#include "Containers/Vector.h"

namespace Azura {

class Drawable {

public:
  Drawable(Memory::Allocator& allocator);
  virtual ~Drawable() = default;

  Drawable(const Drawable& other) = delete;
  Drawable(Drawable&& other) noexcept = default;
  Drawable& operator=(const Drawable& other) = delete;
  Drawable& operator=(Drawable&& other) noexcept = default;

  void SetVertexDataCount(U32 count);
  void SetInstanceDataCount(U32 count);
  virtual void AddVertexData(const Containers::Vector<U8>& buffer, Containers::Vector<RawStorageFormat> strides) = 0;
  virtual void AddInstanceData(const Containers::Vector<U8>& buffer, Containers::Vector<RawStorageFormat> strides) = 0;
  virtual void SetIndexData(const Containers::Vector<U8>& buffer, RawStorageFormat stride)                       = 0;
  virtual void SetDrawMode(DrawType drawMode);

  U32 GetVertexCount() const;
  U32 GetIndexCount() const;
  U32 GetInstanceCount() const;

  void SetVertexCount(U32 count);
  void SetInstanceCount(U32 count);

protected:
  void SetIndexCount(U32 count);

  Memory::Allocator& GetAllocator() const;

  Containers::Vector<BufferInfo> m_vertexBufferInfos;
  Containers::Vector<BufferInfo> m_instanceBufferInfos;
  BufferInfo m_indexBufferInfo;

private:
  // Shared as they are editable by APIs
  U32 m_vertexCount;
  U32 m_indexCount;
  U32 m_instanceCount;

  DrawType m_drawMode;
  std::reference_wrapper<Memory::Allocator> m_allocator;
};

struct DrawablePoolCreateInfo {
  U32 m_byteSize;
  U32 m_numDrawables;
};

class DrawablePool {
public:
  explicit DrawablePool(U32 byteSize, Memory::Allocator& allocator);
  virtual ~DrawablePool() = default;

  DrawablePool(const DrawablePool& other) = delete;
  DrawablePool(DrawablePool&& other) noexcept = default;
  DrawablePool& operator=(const DrawablePool& other) = delete;
  DrawablePool& operator=(DrawablePool&& other) noexcept = default;

  virtual Drawable& CreateDrawable() = 0;

  virtual void AppendBytes(const Containers::Vector<U8>& buffer) = 0;

protected:
  U32 GetOffset() const;
  U32 GetSize() const;
  Memory::Allocator& GetAllocator() const;

private:
  U32 m_byteSize;
  U32 m_offset;
  std::reference_wrapper<Memory::Allocator> m_allocator;
};

} // namespace Azura
