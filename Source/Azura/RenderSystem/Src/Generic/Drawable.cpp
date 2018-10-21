#include "Generic/Drawable.h"
#include <algorithm>
#include "Log/Log.h"
#include "Generic/TextureManager.h"
#include "Containers/Vector.h"

namespace Azura {

namespace {
bool IsPerVertexSlot(const VertexSlot& slot) {
  return slot.m_rate == BufferUsageRate::PerVertex;
}

bool IsPerInstanceSlot(const VertexSlot& slot) {
  return slot.m_rate == BufferUsageRate::PerInstance;
}
} // namespace

Drawable::Drawable(const DrawableCreateInfo& info,
                   U32 numVertexSlots,
                   U32 numInstanceSlots,
                   U32 numUniformSlots,
                   Memory::Allocator& allocator)
  : m_vertexBufferInfos(numVertexSlots, allocator),
    m_instanceBufferInfos(numInstanceSlots, allocator),
    m_uniformBufferInfos(numUniformSlots, allocator),
    m_vertexCount(info.m_vertexCount),
    m_indexCount(info.m_indexCount),
    m_indexType(info.m_indexType),
    m_instanceCount(info.m_instanceCount),
    m_allocator(allocator) {
}

Memory::Allocator& Drawable::GetAllocator() const {
  return m_allocator.get();
}

void Drawable::AddVertexBufferInfo(BufferInfo&& info) {
  m_vertexBufferInfos.PushBack(info);
}

void Drawable::AddInstanceBufferInfo(BufferInfo&& info) {
  m_instanceBufferInfos.PushBack(info);
}

void Drawable::AddUniformBufferInfo(UniformBufferInfo&& info) {
  m_uniformBufferInfos.PushBack(info);
}

U32 Drawable::GetSingleUniformBufferInfo(const DescriptorSlot& slot) {
  U32 idx = 0;
  for(const auto& ubInfo : m_uniformBufferInfos)
  {
    if (ubInfo.m_binding == slot.m_bindIdx && ubInfo.m_set == slot.m_setIdx)
    {
      return idx;
    }

    ++idx;
  }

  throw std::runtime_error("Tried to Update Slot that was never bound");
}

void Drawable::SetIndexBufferInfo(BufferInfo&& info) {
  m_indexBufferInfo = info;
}

U32 Drawable::GetVertexCount() const {
  return m_vertexCount;
}

U32 Drawable::GetIndexCount() const {
  return m_indexCount;
}

U32 Drawable::GetInstanceCount() const {
  return m_instanceCount;
}

RawStorageFormat Drawable::GetIndexType() const {
  return m_indexType;
}

const Containers::Vector<BufferInfo>& Drawable::GetVertexBufferInfos() const {
  return m_vertexBufferInfos;
}

const Containers::Vector<BufferInfo>& Drawable::GetInstanceBufferInfos() const {
  return m_instanceBufferInfos;
}

const Containers::Vector<UniformBufferInfo>& Drawable::GetUniformBufferInfos() const {
  return m_uniformBufferInfos;
}

const BufferInfo& Drawable::GetIndexBufferInfo() const {
  return m_indexBufferInfo;
}

DrawablePoolCreateInfo::DrawablePoolCreateInfo(Memory::Allocator& alloc)
  : m_renderPasses(alloc) {
}

U32 DrawablePoolCreateInfo::AddInputSlot(const VertexSlot& slotInfo) {
  const auto id = U32(m_vertexDataSlots.size());
  m_vertexDataSlots.push_back(slotInfo);
  return id;
}

DrawablePool::DrawablePool(const DrawablePoolCreateInfo& createInfo,
                           DescriptorCount descriptorCount,
                           Memory::Allocator& allocator)
  : m_numVertexSlots(U32(std::count_if(createInfo.m_vertexDataSlots.begin(), createInfo.m_vertexDataSlots.end(),
                                   IsPerVertexSlot))),
    m_numInstanceSlots(U32(std::count_if(createInfo.m_vertexDataSlots.begin(), createInfo.m_vertexDataSlots.end(),
                                     IsPerInstanceSlot))),
    m_descriptorCount(descriptorCount),
    m_vertexDataSlots(allocator),
    m_renderPasses(createInfo.m_renderPasses, allocator),
    m_textureBufferInfos(allocator),
    m_samplerInfos(allocator),
    m_bufferUpdates(allocator),
    m_cullMode(createInfo.m_cullMode),
    m_byteSize(createInfo.m_byteSize),
    m_drawType(createInfo.m_drawType),
    m_allocator(allocator) {

  m_vertexDataSlots.Reserve(U32(createInfo.m_vertexDataSlots.size()));

  for(auto& slot : createInfo.m_vertexDataSlots)
  {
    m_vertexDataSlots.PushBack(slot);

    for(const auto& stride : slot.m_stride)
    {
      m_vertexDataSlots.Last().m_strideSize += GetFormatSize(stride.m_format);
    }
  }

  m_bufferUpdates.Reserve(m_descriptorCount.m_numSampledImageSlots + m_descriptorCount.m_numUniformSlots);

  m_textureBufferInfos.Reserve(m_descriptorCount.m_numSampledImageSlots);
  m_samplerInfos.Reserve(m_descriptorCount.m_numSamplerSlots);
}

void DrawablePool::BindVertexData(DrawableID drawableId, SlotID slot, const Containers::Vector<U8>& buffer) {
  BindVertexData(drawableId, slot, buffer.Data(), buffer.GetSize());
}

void DrawablePool::BindInstanceData(DrawableID drawableId, SlotID slot, const Containers::Vector<U8>& buffer) {
  BindInstanceData(drawableId, slot, buffer.Data(), buffer.GetSize());
}

void DrawablePool::BindUniformData(DrawableID drawableId, SlotID slot, const Containers::Vector<U8>& buffer) {
  BindUniformData(drawableId, slot, buffer.Data(), buffer.GetSize());
}

void DrawablePool::SetIndexData(DrawableID drawableId, const Containers::Vector<U8>& buffer) {
  SetIndexData(drawableId, buffer.Data(), buffer.GetSize());
}

void DrawablePool::UpdateUniformData(DrawableID drawableId, SlotID slot, const Containers::Vector<U8>& buffer) {
  UpdateUniformData(drawableId, slot, buffer.Data(), buffer.GetSize());
}

U32 DrawablePool::GetSize() const {
  return m_byteSize;
}

Memory::Allocator& DrawablePool::GetAllocator() const {
  return m_allocator;
}

DrawType DrawablePool::GetDrawType() const {
  return m_drawType;
}

bool DrawablePool::CanRenderInPass(U32 renderPassId) const {
  for (const auto& pass : m_renderPasses) {
    if (pass == renderPassId) {
      return true;
    }
  }

  return false;
}
} // namespace Azura
