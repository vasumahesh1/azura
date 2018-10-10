#pragma once
#include "Containers/Vector.h"
#include "GenericTypes.h"
#include "Types.h"
#include "Core/RawStorageFormat.h"

namespace Azura {
struct TextureDesc;
using DrawableID = U32;
class Shader;

struct DrawableCreateInfo {
  U32 m_vertexCount{0};
  U32 m_instanceCount{0};
  U32 m_indexCount{0};
  RawStorageFormat m_indexType{RawStorageFormat::R32_UINT};
};

class Drawable {
public:
  Drawable(const DrawableCreateInfo& info, U32 numVertexSlots, U32 numInstanceSlots, U32 numUniformSlots, Memory::Allocator& allocator);
  virtual ~Drawable() = default;

  Drawable(const Drawable& other) = delete;
  Drawable(Drawable&& other) noexcept = default;
  Drawable& operator=(const Drawable& other) = delete;
  Drawable& operator=(Drawable&& other) noexcept = delete;

  void AddVertexBufferInfo(BufferInfo&& info);
  void AddInstanceBufferInfo(BufferInfo&& info);
  void AddUniformBufferInfo(UniformBufferInfo&& info);
  void SetIndexBufferInfo(BufferInfo&& info);

  U32 GetVertexCount() const;
  U32 GetIndexCount() const;
  U32 GetInstanceCount() const;
  RawStorageFormat GetIndexType() const;

  const Containers::Vector<BufferInfo>& GetVertexBufferInfos() const;
  const Containers::Vector<BufferInfo>& GetInstanceBufferInfos() const;
  const Containers::Vector<UniformBufferInfo>& GetUniformBufferInfos() const;
  const BufferInfo& GetIndexBufferInfo() const;

protected:
  Memory::Allocator& GetAllocator() const;

  Containers::Vector<BufferInfo> m_vertexBufferInfos;
  Containers::Vector<BufferInfo> m_instanceBufferInfos;
  Containers::Vector<UniformBufferInfo> m_uniformBufferInfos;
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
  CullMode m_cullMode{CullMode::BackBit};

  DrawType m_drawType{DrawType::InstancedIndexed};

  Containers::Vector<VertexSlot> m_vertexDataSlots;
  Containers::Vector<U32> m_renderPasses;

  ~DrawablePoolCreateInfo() = default;

  DrawablePoolCreateInfo(const DrawablePoolCreateInfo& other) = delete;
  DrawablePoolCreateInfo(DrawablePoolCreateInfo&& other) noexcept = default;
  DrawablePoolCreateInfo& operator=(const DrawablePoolCreateInfo& other) = delete;
  DrawablePoolCreateInfo& operator=(DrawablePoolCreateInfo&& other) noexcept = default;
  
  DrawablePoolCreateInfo(Memory::Allocator& alloc);
};

class DrawablePool {
public:
  explicit DrawablePool(const DrawablePoolCreateInfo& createInfo, DescriptorCount descriptorCount, Memory::Allocator& allocator);
  virtual ~DrawablePool() = default;

  DrawablePool(const DrawablePool& other) = delete;
  DrawablePool(DrawablePool&& other) noexcept = default;
  DrawablePool& operator=(const DrawablePool& other) = delete;
  DrawablePool& operator=(DrawablePool&& other) noexcept = default;

  virtual DrawableID CreateDrawable(const DrawableCreateInfo& createInfo) = 0;

  virtual void AddBufferBinding(SlotID slot, const Containers::Vector<RawStorageFormat>& strides) = 0;

  // Drawable Scope Binds
  void BindVertexData(DrawableID drawableId, SlotID slot, const Containers::Vector<U8>& buffer);
  virtual void BindVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) = 0;

  void BindInstanceData(DrawableID drawableId, SlotID slot, const Containers::Vector<U8>& buffer);
  virtual void BindInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) = 0;

  void BindUniformData(DrawableID drawableId, SlotID slot, const Containers::Vector<U8>& buffer);
  virtual void BindUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) = 0;

  void SetIndexData(DrawableID drawableId, const Containers::Vector<U8>& buffer);
  virtual void SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) = 0;

  // Pool Scope Binds
  virtual void BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) = 0;
  virtual void BindSampler(SlotID slot, const SamplerDesc& desc) = 0;

  virtual void Submit() = 0;

  U32 GetSize() const;

protected:
  Memory::Allocator& GetAllocator() const;
  DrawType GetDrawType() const;

  int GetVertexSlotIndex(SlotID id) const;
  int GetDescriptorSlotIndex(SlotID id) const;

  U32 m_numVertexSlots;
  U32 m_numInstanceSlots;

  DescriptorCount m_descriptorCount;

  Containers::Vector<VertexSlot> m_vertexDataSlots;

  Containers::Vector<TextureBufferInfo> m_textureBufferInfos;
  Containers::Vector<SamplerInfo> m_samplerInfos;

  CullMode m_cullMode;

private:
  U32 m_byteSize;
  DrawType m_drawType;

  std::reference_wrapper<Memory::Allocator> m_allocator;
};

} // namespace Azura
