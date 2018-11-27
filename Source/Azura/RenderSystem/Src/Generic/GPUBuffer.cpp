#include "Generic/GPUBuffer.h"

namespace Azura {
namespace {
SizeType Align(SizeType uLocation, SizeType uAlign) {
  if ((0 == uAlign) || (uAlign & (uAlign - 1)) != 0) {
    throw std::runtime_error("Non Power of 2 alignment");
  }

  return ((uLocation + (uAlign - 1)) & ~(uAlign - 1));
}
} // namespace

BufferEntry::BufferEntry(U32 offset, U32 size, U32 maxSize, DescriptorType type)
  : m_offset(offset),
    m_size(size),
    m_maxSize(maxSize),
    m_type(type) {
}

GPUBuffer::GPUBuffer(U32 size)
  : m_byteSize(size) {
}

MappedGPUBuffer::MappedGPUBuffer(U8* beginPtr, U32 maxEntries, U32 size, Memory::Allocator& allocator)
  : GPUBuffer(size),
    p_dataCur(beginPtr),
    p_dataBeg(beginPtr),
    // NOLINTNEXTLINE
    p_dataEnd(beginPtr + size),
    m_entries(maxEntries, allocator) {
  assert(p_dataBeg != nullptr);
  assert(p_dataCur != nullptr);
  assert(size != 0);
  assert(maxEntries != 0);
}

bool MappedGPUBuffer::SubAllocate(U32 size, U32 alignment) {
  assert(size != 0);
  assert(alignment != 0);

  p_dataCur = reinterpret_cast<U8*>(Align(reinterpret_cast<SizeType>(p_dataCur), SizeType(alignment))); // NOLINT
  return (p_dataCur + size > p_dataEnd) ? false : true;                                                 // NOLINT
}
} // namespace Azura
