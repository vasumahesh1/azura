#include "Generic/Drawable.h"

namespace Azura {

Drawable::Drawable(Memory::Allocator& allocator)
  : m_vertexBufferInfos(allocator),
    m_instanceBufferInfos(allocator),
    m_vertexCount(0),
    m_indexCount(0),
    m_instanceCount(0),
    m_drawMode(DrawType::InstancedIndexed),
    m_allocator(allocator) {
}

void Drawable::SetVertexDataCount(const U32 count) {
  m_vertexBufferInfos.Reserve(count);
}

void Drawable::SetInstanceDataCount(U32 count) {
  m_instanceBufferInfos.Reserve(count);
}

void Drawable::SetDrawMode(const DrawType drawMode) {
  m_drawMode = drawMode;
}

Memory::Allocator& Drawable::GetAllocator() const {
  return m_allocator.get();
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

void Drawable::SetVertexCount(U32 count) {
  m_vertexCount = count;
}

void Drawable::SetIndexCount(U32 count) {
  m_indexCount = count;
}

void Drawable::SetInstanceCount(U32 count) {
  m_instanceCount = count;
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

DrawablePool::DrawablePool(const U32 byteSize, Memory::Allocator& allocator)
  : m_byteSize(byteSize),
    m_offset(0),
    m_allocator(allocator) {
}

void DrawablePool::AppendBytes(const Containers::Vector<U8>& buffer) { m_offset += buffer.GetSize(); }

U32 DrawablePool::GetOffset() const {
  return m_offset;
}

U32 DrawablePool::GetSize() const {
  return m_byteSize;
}

Memory::Allocator& DrawablePool::GetAllocator() const {
  return m_allocator;
}
} // namespace Azura
