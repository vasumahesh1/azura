

#include "Memory/MonotonicAllocator.h"
#include <cassert>
#include "Utils/Macros.h"
#include "Memory/MemoryBuffer.h"

namespace Azura {
  namespace Memory {

namespace {
/**
* \brief Finds the next largest Aligned multiple according to the input size
* \param size Size of Bytes to Allocate
* \param alignment Alignment of the data to allocate
* \return Next greatest alignment multiple that can fit `size` bytes
*/
SizeType AlignAhead(SizeType size, SizeType alignment) {
  return (size + (alignment - 1)) & ~(alignment - 1);
}
} // namespace

MonotonicAllocator::MonotonicAllocator(MemoryBuffer& buffer, U32 size)
  : Allocator(buffer.Allocate(size), size),
    m_headPtr(BasePtr()),
    m_sourceBuffer(buffer) {
}

MonotonicAllocator::~MonotonicAllocator() {
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc, cppcoreguidelines-pro-type-reinterpret-cast)
  m_sourceBuffer.Deallocate(reinterpret_cast<void*>(BasePtr()));
}

void* MonotonicAllocator::Allocate(U32 size, U32 alignment) {

  // Power of 2 check
  assert((alignment & (alignment - 1)) == 0);

  const SizeType alignedSize = AlignAhead(size, alignment);
  assert(Size() - (m_headPtr - BasePtr()) >= alignedSize);

  const SizeType mask          = alignment - 1;
  const UPTR misalignment      = (m_headPtr & mask);

  // TODO(vasumahesh1): Need to figure this out.
  const AddressDiff adjustment = [&] ()
  {
    if (misalignment > 0)
    {
      return alignment - misalignment;
    }

    return AddressPtr(0);
  } ();

  const UPTR addr  = m_headPtr + adjustment;
  m_headPtr = addr + alignedSize;

  // TODO(vasumahesh1): Store adjusment appropriately for deallocation
  // assert(adjustment < 256);
  // Byte* pAlignedMem = reinterpret_cast<Byte*>(alignedAddress);
  // pAlignedMem[-1] = static_cast<Byte>(adjustment);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<void*>(addr);
}

void MonotonicAllocator::Deallocate(void* address) {
  UNUSED(address);
}

void MonotonicAllocator::Reset() {
  m_headPtr = BasePtr();
}

  } // namespace Memory
} // namespace Azura
