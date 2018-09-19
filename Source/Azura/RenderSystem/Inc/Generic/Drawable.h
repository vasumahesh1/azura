#pragma once
#include "Containers/Vector.h"
#include "GenericTypes.h"
#include "Types.h"
#include "RawStorageFormat.h"

namespace Azura {
using DrawableID = U32;

class Shader;

struct DrawableCreateInfo {
  U32 m_vertexCount{0};
  U32 m_instanceCount{0};
  U32 m_indexCount{0};
  RawStorageFormat m_indexType{RawStorageFormat::R32_UINT};
};

struct DrawablePoolSlotInfo {
  U32 m_numVertexSlots{0};
  U32 m_numInstanceSlots{0};
  U32 m_numUniformSlots{0};
};

class Drawable {
public:
  Drawable(const DrawableCreateInfo& info, const DrawablePoolSlotInfo& slotInfo, Memory::Allocator& allocator);
  virtual ~Drawable() = default;

  Drawable(const Drawable& other) = delete;
  Drawable(Drawable&& other) noexcept = default;
  Drawable& operator=(const Drawable& other) = delete;
  Drawable& operator=(Drawable&& other) noexcept = delete;

  void AddVertexBufferInfo(BufferInfo&& info);
  void AddInstanceBufferInfo(BufferInfo&& info);
  void AddUniformBufferInfo(BufferInfo&& info);
  void SetIndexBufferInfo(BufferInfo&& info);

  U32 GetVertexCount() const;
  U32 GetIndexCount() const;
  U32 GetInstanceCount() const;
  RawStorageFormat GetIndexType() const;

  const Containers::Vector<BufferInfo>& GetVertexBufferInfos() const;
  const Containers::Vector<BufferInfo>& GetInstanceBufferInfos() const;
  const BufferInfo& GetIndexBufferInfo() const;

  virtual void Submit() = 0;

protected:
  Memory::Allocator& GetAllocator() const;

  Containers::Vector<BufferInfo> m_vertexBufferInfos;
  Containers::Vector<BufferInfo> m_instanceBufferInfos;
  Containers::Vector<BufferInfo> m_uniformBufferInfos;
  BufferInfo m_indexBufferInfo;
  DrawableCreateInfo m_createInfo;

private:
  // Shared as they are editable by APIs
  const U32 m_vertexCount;
  const U32 m_indexCount;
  const RawStorageFormat m_indexType;

  U32 m_instanceCount;

  std::reference_wrapper<Memory::Allocator> m_allocator;
};

struct DrawablePoolCreateInfo {
  U32 m_byteSize{0};
  U32 m_numDrawables{0};
  U32 m_numShaders{0};
  DrawablePoolSlotInfo m_slotInfo{};
  DrawType m_drawType{DrawType::InstancedIndexed};
};

class DrawablePool {
public:
  explicit DrawablePool(const DrawablePoolCreateInfo& createInfo, Memory::Allocator& allocator);
  virtual ~DrawablePool() = default;

  DrawablePool(const DrawablePool& other) = delete;
  DrawablePool(DrawablePool&& other) noexcept = default;
  DrawablePool& operator=(const DrawablePool& other) = delete;
  DrawablePool& operator=(DrawablePool&& other) noexcept = default;

  virtual DrawableID CreateDrawable(const DrawableCreateInfo& createInfo) = 0;

  virtual void AddShader(const Shader& shader) = 0;

  virtual void AddBufferBinding(Slot slot, const Containers::Vector<RawStorageFormat>& strides) = 0;

  void BindVertexData(DrawableID drawableId, const Slot& slot, const Containers::Vector<U8>& buffer);
  virtual void BindVertexData(DrawableID drawableId, const Slot& slot, const U8* buffer, U32 size) = 0;

  void BindInstanceData(DrawableID drawableId, const Slot& slot, const Containers::Vector<U8>& buffer);
  virtual void BindInstanceData(DrawableID drawableId, const Slot& slot, const U8* buffer, U32 size) = 0;

  void BindUniformData(DrawableID drawableId, const Slot& slot, const Containers::Vector<U8>& buffer);
  virtual void BindUniformData(DrawableID drawableId, const Slot& slot, const U8* buffer, U32 size) = 0;

  void SetIndexData(DrawableID drawableId, const Containers::Vector<U8>& buffer);
  virtual void SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) = 0;

  virtual void Submit() = 0;

  U32 GetSize() const;

protected:
  Memory::Allocator& GetAllocator() const;
  DrawType GetDrawType() const;
  const DrawablePoolSlotInfo& GetSlotInfo() const;

private:
  U32 m_byteSize;
  DrawType m_drawType;
  const DrawablePoolSlotInfo m_slotInfo;

  std::reference_wrapper<Memory::Allocator> m_allocator;
};

} // namespace Azura
