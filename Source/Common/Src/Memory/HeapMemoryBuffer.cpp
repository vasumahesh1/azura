#include "Memory/HeapMemoryBuffer.h"
#include <cstdlib>
#include <cassert>

#include "Utils/Macros.h"

namespace Azura {
namespace Memory {
HeapMemoryBuffer::HeapMemoryBuffer(const SizeType blockSize)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, cppcoreguidelines-no-malloc)
  : MemoryBuffer(blockSize, reinterpret_cast<AddressPtr>(malloc(blockSize))) {
}

HeapMemoryBuffer::HeapMemoryBuffer(SizeType blockSize, SizeType alignment)
  : MemoryBuffer(blockSize,
                 // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, cppcoreguidelines-no-malloc)
                 reinterpret_cast<AddressPtr>(ALIGNED_ALLOC(alignment, blockSize
                 ))) {
}

HeapMemoryBuffer::~HeapMemoryBuffer() {
  if (m_memoryBlock != 0u) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc, cppcoreguidelines-pro-type-reinterpret-cast)
    free(reinterpret_cast<void*>(m_memoryBlock));
  }
}

void* HeapMemoryBuffer::Allocate(SizeType size) {
  const AddressPtr addr = AllocateRaw(size);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<void*>(addr);
}

void HeapMemoryBuffer::Deallocate(void* address) {
  UNUSED(address);
}

UPTR HeapMemoryBuffer::AllocateRaw(SizeType size) {
  // Available > Size
  assert(mSize - (m_currentPosition - m_memoryBlock) >= size);

  const AddressPtr addr = m_currentPosition;
  m_currentPosition += size;

  return addr;
}
} // namespace Memory
} // namespace Azura
