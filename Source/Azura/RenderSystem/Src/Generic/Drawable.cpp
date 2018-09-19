#include "Generic/Drawable.h"

namespace Azura {

Drawable::Drawable(const DrawableCreateInfo& info, const DrawablePoolSlotInfo& slotInfo, Memory::Allocator& allocator)
  : m_vertexBufferInfos(slotInfo.m_numVertexSlots, allocator),
    m_instanceBufferInfos(slotInfo.m_numInstanceSlots, allocator),
    m_uniformBufferInfos(slotInfo.m_numUniformSlots, allocator),
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

DrawablePool::DrawablePool(const DrawablePoolCreateInfo& createInfo, Memory::Allocator& allocator)
  : m_byteSize(createInfo.m_byteSize),
    m_drawType(createInfo.m_drawType),
    m_slotInfo(createInfo.m_slotInfo),
    m_allocator(allocator) {
}

void DrawablePool::BindVertexData(DrawableID drawableId, const Slot& slot, const Containers::Vector<U8>& buffer) {
  BindVertexData(drawableId, slot, buffer.Data(), buffer.GetSize());
}

void DrawablePool::BindInstanceData(DrawableID drawableId, const Slot& slot, const Containers::Vector<U8>& buffer) {
  BindInstanceData(drawableId, slot, buffer.Data(), buffer.GetSize());
}

void DrawablePool::BindUniformData(DrawableID drawableId, const Slot& slot, const Containers::Vector<U8>& buffer) {
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

const DrawablePoolSlotInfo& DrawablePool::GetSlotInfo() const {
  return m_slotInfo;
}
} // namespace Azura
