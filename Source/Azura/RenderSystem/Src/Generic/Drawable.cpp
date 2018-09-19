#include "Generic/Drawable.h"

namespace Azura {

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
  : m_uniformBuffers(alloc) {
}

DrawablePool::DrawablePool(const DrawablePoolCreateInfo& createInfo, Memory::Allocator& allocator)
  : m_byteSize(createInfo.m_byteSize),
    m_drawType(createInfo.m_drawType),
    m_numVertexSlots(createInfo.m_slotInfo.m_numVertexSlots),
    m_numInstanceSlots(createInfo.m_slotInfo.m_numInstanceSlots),
    m_numUniformSlots(createInfo.m_uniformBuffers.GetSize()),
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

} // namespace Azura
