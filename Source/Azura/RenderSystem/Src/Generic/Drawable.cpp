#include "Generic/Drawable.h"

namespace Azura {
DataBuffer::DataBuffer(Memory::Allocator& allocator)
  : m_info(),
    m_data(allocator) {
}

DrawableCreateInfo::DrawableCreateInfo(Memory::Allocator& allocator)
  : m_buffers(allocator) {
}

Drawable::Drawable(U32 vertexCount, U32 indexCount, Memory::Allocator& allocator)
  : m_vertexCount(vertexCount),
    m_indexCount(indexCount),
    m_bufferInfos(allocator) {
}

U32 Drawable::VertexCount() const {
  return m_vertexCount;
}

U32 Drawable::IndexCount() const {
  return m_indexCount;
}

DrawablePool::DrawablePool(U32 byteSize)
  : m_byteSize(byteSize) {
}

U32 DrawablePool::Size() const {
  return m_byteSize;
}
} // namespace Azura
