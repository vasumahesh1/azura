﻿#pragma once
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
  Drawable(const DrawableCreateInfo& info,
           U32 numVertexSlots,
           U32 numInstanceSlots,
           U32 numUniformSlots,
           Memory::Allocator& allocator);
  virtual ~Drawable() = default;

  Drawable(const Drawable& other) = delete;
  Drawable(Drawable&& other) noexcept = default;
  Drawable& operator=(const Drawable& other) = delete;
  Drawable& operator=(Drawable&& other) noexcept = delete;

  void AddVertexBufferInfo(BufferInfo&& info);
  void AddInstanceBufferInfo(BufferInfo&& info);
  void AddUniformBufferInfo(UniformBufferInfo&& info);
  U32 GetSingleUniformBufferInfo(const DescriptorSlot & slot) const;
  U32 GetSingleVertexBufferInfo(const SlotID & slot) const;
  U32 GetSingleInstanceBufferInfo(const SlotID & slot) const;
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
  CullMode m_cullMode{CullMode::None};

  DrawType m_drawType{DrawType::InstancedIndexed};

  SmallVector<VertexSlot, 4> m_vertexDataSlots;
  Containers::Vector<U32> m_renderPasses;

  ~DrawablePoolCreateInfo() = default;

  DrawablePoolCreateInfo(const DrawablePoolCreateInfo& other) = delete;
  DrawablePoolCreateInfo(DrawablePoolCreateInfo&& other) noexcept = delete;
  DrawablePoolCreateInfo& operator=(const DrawablePoolCreateInfo& other) = delete;
  DrawablePoolCreateInfo& operator=(DrawablePoolCreateInfo&& other) noexcept = delete;

  DrawablePoolCreateInfo(Memory::Allocator& alloc);

  U32 AddInputSlot(const VertexSlot& slotInfo);
};

class DrawablePool {
public:
  explicit DrawablePool(const DrawablePoolCreateInfo& createInfo,
                        DescriptorCount descriptorCount,
                        Memory::Allocator& allocator);
  virtual ~DrawablePool() = default;

  DrawablePool(const DrawablePool& other) = delete;
  DrawablePool(DrawablePool&& other) noexcept = default;
  DrawablePool& operator=(const DrawablePool& other) = delete;
  DrawablePool& operator=(DrawablePool&& other) noexcept = default;

  virtual DrawableID CreateDrawable(const DrawableCreateInfo& createInfo) = 0;

  // Drawable Scope Binds
  void BindVertexData(DrawableID drawableId, SlotID slot, const Containers::Vector<U8>& buffer);
  virtual void BindVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) = 0;
  virtual void BindVertexData(DrawableID drawableId, SlotID slot, U32 sourceBuffer, U32 offset, U32 size) = 0;

  void BindInstanceData(DrawableID drawableId, SlotID slot, const Containers::Vector<U8>& buffer);
  virtual void BindInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) = 0;

  void BindUniformData(DrawableID drawableId, SlotID slot, const Containers::Vector<U8>& buffer);
  virtual void BindUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) = 0;

  void SetIndexData(DrawableID drawableId, const Containers::Vector<U8>& buffer);
  virtual void SetIndexData(DrawableID drawableId, const U8* buffer, U32 size) = 0;

  // Pool Scope Binds
  virtual void AddShader(U32 shaderId) = 0;
  virtual void BindTextureData(SlotID slot, const TextureDesc& desc, const U8* buffer) = 0;
  virtual void BindSampler(SlotID slot, const SamplerDesc& desc) = 0;

  virtual void BeginUpdates() = 0;
  void UpdateUniformData(DrawableID drawableId, SlotID slot, const Containers::Vector<U8>& buffer);
  virtual void UpdateUniformData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) = 0;
  virtual void UpdateVertexData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) = 0;
  virtual void UpdateInstanceData(DrawableID drawableId, SlotID slot, const U8* buffer, U32 size) = 0;
  virtual void UpdateTextureData(SlotID slot, const U8* buffer) = 0;

  U32 GetSingleTextureBufferInfo(const DescriptorSlot & slot);

  virtual void Submit() = 0;
  virtual void SubmitUpdates() = 0;

  U32 GetSize() const;
  bool CanRenderInPass(U32 renderPassId) const;

protected:
  Memory::Allocator& GetAllocator() const;
  DrawType GetDrawType() const;

  U32 m_numVertexSlots;
  U32 m_numInstanceSlots;

  DescriptorCount m_descriptorCount;

  Containers::Vector<VertexSlot> m_vertexDataSlots;
  Containers::Vector<U32> m_renderPasses;

  Containers::Vector<TextureBufferInfo> m_textureBufferInfos;
  Containers::Vector<SamplerInfo> m_samplerInfos;

  Containers::Vector<BufferUpdate> m_bufferUpdates;

  CullMode m_cullMode;

private:
  U32 m_byteSize;
  DrawType m_drawType;

  std::reference_wrapper<Memory::Allocator> m_allocator;
};

} // namespace Azura
