#include "Memory/MemoryBuffer.h"
#include <cassert>

namespace Azura {
namespace Memory {
MemoryBuffer::MemoryBuffer(U32 size)
  : m_size(size) {
}

MemoryBuffer::MemoryBuffer(U32 size, AddressPtr blockStart)
  : m_size(size),
    m_memoryBlock(blockStart),
    m_currentPosition(blockStart) {
}

MemoryBuffer::MemoryBuffer(MemoryBuffer&& other) noexcept
  : m_memoryBlock(other.m_memoryBlock),
    m_currentPosition(other.m_currentPosition),
    m_size(other.m_size) {
  other.m_memoryBlock     = 0;
  other.m_currentPosition = 0;
  other.m_size            = 0;
}

MemoryBuffer& MemoryBuffer::operator=(MemoryBuffer&& other) noexcept {
  assert(this != &other);

  m_memoryBlock     = other.m_memoryBlock;
  m_currentPosition = other.m_currentPosition;
  m_size            = other.m_size;

  other.m_memoryBlock     = 0;
  other.m_currentPosition = 0;
  other.m_size            = 0;

  return *this;
}
} // namespace Memory
} // namespace Azura
