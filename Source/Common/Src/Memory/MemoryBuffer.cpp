#include "Memory/MemoryBuffer.h"
#include <cassert>

namespace Azura {
namespace Memory {
MemoryBuffer::MemoryBuffer(SizeType size)
  : mSize(size) {
}

MemoryBuffer::MemoryBuffer(SizeType size, AddressPtr blockStart)
  : mSize(size),
    m_memoryBlock(blockStart),
    m_currentPosition(blockStart) {
}

MemoryBuffer::MemoryBuffer(MemoryBuffer&& other) noexcept
  : m_memoryBlock(other.m_memoryBlock),
    m_currentPosition(other.m_currentPosition),
    mSize(other.mSize) {
  other.m_memoryBlock     = 0;
  other.m_currentPosition = 0;
  other.mSize            = 0;
}

MemoryBuffer& MemoryBuffer::operator=(MemoryBuffer&& other) noexcept {
  assert(this != &other);

  m_memoryBlock     = other.m_memoryBlock;
  m_currentPosition = other.m_currentPosition;
  mSize            = other.mSize;

  other.m_memoryBlock     = 0;
  other.m_currentPosition = 0;
  other.mSize            = 0;

  return *this;
}
} // namespace Memory
} // namespace Azura
