#include "Memory/HeapMemoryBuffer.h"
#include <cstdlib>
#include <cassert>

#include "Utils/Macros.h"

namespace Azura {
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
  if (mMemoryBlock != 0u) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc, cppcoreguidelines-pro-type-reinterpret-cast)
    free(reinterpret_cast<void*>(mMemoryBlock));
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
  assert(mSize - (mCurrentPosition - mMemoryBlock) >= size);

  const AddressPtr addr = mCurrentPosition;
  mCurrentPosition += size;

  return addr;
}
} // namespace Azura
