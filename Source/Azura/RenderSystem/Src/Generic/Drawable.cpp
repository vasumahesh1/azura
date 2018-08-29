#include "Generic/Drawable.h"

namespace Azura {

Drawable::Drawable(Memory::Allocator& allocator)
  : m_vertexBufferInfos(allocator),
    m_instanceBufferInfos(allocator),
    m_indexBufferInfo(allocator),
    m_vertexCount(0),
    m_indexCount(0),
    m_instanceCount(0),
    m_drawMode(DrawType::InstancedIndexed),
    m_allocator(allocator) {
}

void Drawable::SetVertexDataCount(const U32 count) { m_vertexBufferInfos.Reserve(count); }

void Drawable::SetInstanceDataCount(U32 count) { m_instanceBufferInfos.Reserve(count); }

void Drawable::SetDrawMode(const DrawType drawMode) { m_drawMode = drawMode; }

Memory::Allocator& Drawable::GetAllocator() const { return m_allocator.get(); }

U32 Drawable::GetVertexCount() const { return m_vertexCount; }

U32 Drawable::GetIndexCount() const { return m_indexCount; }

U32 Drawable::GetInstanceCount() const { return m_instanceCount; }

void Drawable::SetVertexCount(U32 count) { m_vertexCount = count; }

void Drawable::SetIndexCount(U32 count) { m_indexCount = count; }

void Drawable::SetInstanceCount(U32 count) { m_instanceCount = count; }

DrawablePool::DrawablePool(const U32 byteSize, Memory::Allocator& allocator)
  : m_byteSize(byteSize),
    m_offset(0), m_allocator(allocator) {}

U32 DrawablePool::GetOffset() const { return m_offset; }

U32 DrawablePool::GetSize() const {
  return m_byteSize;
}

Memory::Allocator& DrawablePool::GetAllocator() const { return m_allocator; }
} // namespace Azura
