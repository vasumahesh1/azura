#include "Generic/Drawable.h"
#include <algorithm>

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

void Drawable::AddUniformBufferInfo(BufferInfo&& info) {
  m_uniformBufferInfos.PushBack(info);
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

const BufferInfo& Drawable::GetIndexBufferInfo() const {
  return m_indexBufferInfo;
}

DrawablePoolCreateInfo::DrawablePoolCreateInfo(Memory::Allocator& alloc)
  : m_vertexDataSlots(alloc),
    m_vertexStageDescriptorSlots(alloc),
    m_pixelStageDescriptorSlots(alloc) {
}

DrawablePool::DrawablePool(const DrawablePoolCreateInfo& createInfo, Memory::Allocator& allocator)
  : m_numVertexSlots(std::count_if(createInfo.m_vertexDataSlots.Begin(), createInfo.m_vertexDataSlots.End(),
                                   IsPerVertexSlot)),
    m_numInstanceSlots(std::count_if(createInfo.m_vertexDataSlots.Begin(), createInfo.m_vertexDataSlots.End(),
                                     IsPerInstanceSlot)),
    m_descriptorSlots(createInfo.m_vertexStageDescriptorSlots.GetSize() + createInfo.m_pixelStageDescriptorSlots.GetSize(), allocator),
    m_vertexDataSlots(createInfo.m_vertexDataSlots, allocator),
    m_byteSize(createInfo.m_byteSize),
    m_drawType(createInfo.m_drawType),
    m_allocator(allocator) {

  for(const auto& slot : createInfo.m_vertexStageDescriptorSlots)
  {
    m_descriptorSlots.PushBack(slot);
  }

  for(const auto& slot : createInfo.m_pixelStageDescriptorSlots)
  {
    m_descriptorSlots.PushBack(slot);
  }

  m_numUniformSlots = std::count_if(m_descriptorSlots.Begin(), m_descriptorSlots.End(), [](const DescriptorSlot& item) -> bool { return item.m_type == DescriptorType::UniformBuffer; });
  m_numSamplerSlots = std::count_if(m_descriptorSlots.Begin(), m_descriptorSlots.End(), [](const DescriptorSlot& item) -> bool { return item.m_type == DescriptorType::Sampler; });
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

U32 DrawablePool::GetSize() const {
  return m_byteSize;
}

Memory::Allocator& DrawablePool::GetAllocator() const {
  return m_allocator;
}

DrawType DrawablePool::GetDrawType() const {
  return m_drawType;
}

int DrawablePool::GetVertexSlotIndex(SlotID id) const {
  auto it = std::find_if(m_vertexDataSlots.Begin(), m_vertexDataSlots.End(), [id](const VertexSlot& slot) -> bool { return slot.m_key == id; });

  if (it == m_vertexDataSlots.End())
  {
    return -1;
  }

  return it - m_vertexDataSlots.Begin();
}

int DrawablePool::GetDescriptorSlotIndex(SlotID id) const {
  auto it = std::find_if(m_descriptorSlots.Begin(), m_descriptorSlots.End(), [id](const DescriptorSlot& slot) -> bool { return slot.m_key == id; });

  if (it == m_descriptorSlots.End())
  {
    return -1;
  }

  return it - m_descriptorSlots.Begin();
}

} // namespace Azura
