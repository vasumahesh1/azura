

#include "Memory/MonotonicAllocator.h"
#include <cassert>
#include "Utils/Macros.h"

namespace Azura {

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

MonotonicAllocator::MonotonicAllocator(SizeType size)
  : mSize(size),
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, cppcoreguidelines-no-malloc)
    mMemoryBlock(reinterpret_cast<AddressPtr>(ALIGNED_ALLOC(16, mSize))),
    mCurrentPosition(mMemoryBlock) {
}

MonotonicAllocator::MonotonicAllocator(SizeType size, void* managedResource)
  : mSize(size),
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    mMemoryBlock(reinterpret_cast<AddressPtr>(managedResource)),
    mCurrentPosition(mMemoryBlock),
    mIsManagedResource(true) {
}

MonotonicAllocator::~MonotonicAllocator() {
  if (mIsManagedResource) {
    return;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc, cppcoreguidelines-pro-type-reinterpret-cast)
  ALIGNED_FREE(reinterpret_cast<void*>(mMemoryBlock));
}

MonotonicAllocator MonotonicAllocator::Create(SizeType size,
                                              MemoryBuffer& buffer) {
  return MonotonicAllocator(size, buffer.Allocate(size));
}

void* MonotonicAllocator::Allocate(SizeType size, SizeType alignment) {
  // Power of 2 check
  assert((alignment & (alignment - 1)) == 0);

  const SizeType alignedSize = AlignAhead(size, alignment);
  assert(mSize - (mCurrentPosition - mMemoryBlock) >= alignedSize);

  const SizeType mask          = alignment - 1;
  const UPTR misalignment      = (mCurrentPosition & mask);
  const AddressDiff adjustment = alignment - misalignment;

  const UPTR addr  = mCurrentPosition + adjustment;
  mCurrentPosition = addr + alignedSize;

  // TODO(Vasu Mahesh): Store adjusment appropriately for deallocation
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
  mCurrentPosition = mMemoryBlock;
}

} // namespace Azura
