#include "Memory/RangeAllocator.h"

namespace Azura {
namespace Memory {

namespace {
/**
* \brief Finds the next largest Aligned multiple according to the input size
* \param size Size of Bytes to Allocate
* \param alignment Alignment of the data to allocate
* \return Next greatest alignment multiple that can fit `size` bytes
*/
U32 AlignAhead(U32 size, U32 alignment) {
  return (size + (alignment - 1)) & ~(alignment - 1);
}
} // namespace

RangeAllocator::RangeAllocator(MemoryBuffer& buffer, U32 size)
  : Allocator(buffer.Allocate(size), size),
    m_sourceBuffer(buffer) {
}

RangeAllocator::~RangeAllocator() {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  m_sourceBuffer.Deallocate(reinterpret_cast<void*>(BasePtr()));
}

void* RangeAllocator::Allocate(U32 size, U32 alignment) {
  auto finalRangeItr     = m_freeRanges.end();
  U32 finalOffsetInRange = 0;

  const U32 expectedSize = AlignAhead(size, alignment);

  for (auto itr                = m_freeRanges.begin(); itr != m_freeRanges.end(); ++itr) {
    const auto& range          = *itr;
    const AddressPtr rangeBase = BasePtr() + range.m_offset;

    const U32 mask          = alignment - 1;
    const UPTR misalignment = (rangeBase & mask);

    // TODO(vasumahesh1): Check Logic
    const auto misalignmentFromOffset = U32(misalignment - rangeBase);
    const U32 requiredSize = misalignmentFromOffset + expectedSize;

    if (range.m_size >= requiredSize) {
      finalRangeItr      = itr;
      finalOffsetInRange = misalignmentFromOffset;
      break;
    }
  }

  if (finalRangeItr == m_freeRanges.end()) {
    // TODO(vasumahesh1): Must GC
    return nullptr;
  }

  // Take Copy
  const MemoryRange targetRange = *finalRangeItr;

  // Remove Free Range
  m_freeRanges.erase(finalRangeItr);

  // TODO(vasumahesh1): Should Sort or Place as sorted?

  if (finalOffsetInRange > 0) {
    m_freeRanges.emplace_back(targetRange.m_offset, finalOffsetInRange);
  }

  const U32 remainingRangeSize = targetRange.m_size - finalOffsetInRange - expectedSize;

  if (remainingRangeSize > 0) {
    m_freeRanges.emplace_back(targetRange.m_offset + finalOffsetInRange + expectedSize, remainingRangeSize);
  }

  const auto targetOffset = targetRange.m_offset + finalOffsetInRange;
  m_occupiedRanges.emplace_back(targetOffset, expectedSize);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<void*>(static_cast<AddressPtr>(BasePtr() + targetOffset));
}

void RangeAllocator::Deallocate(void* address) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto addressPtr = reinterpret_cast<AddressPtr>(address);
  auto finalRangeItr    = m_occupiedRanges.end();

  for (auto itr                = m_occupiedRanges.begin(); itr != m_occupiedRanges.end(); ++itr) {
    const auto& range          = *itr;
    const AddressPtr rangeBase = BasePtr() + range.m_offset;

    if (rangeBase == addressPtr) {
      finalRangeItr = itr;
      break;
    }
  }

  if (finalRangeItr == m_occupiedRanges.end()) {
    // TODO(vasumahesh1): Must Throw here?
    return;
  }

  // Take Copy
  const MemoryRange targetRange = *finalRangeItr;

  // Remove Free Range
  m_occupiedRanges.erase(finalRangeItr);

  // Release to free ranges
  m_freeRanges.emplace_back(targetRange.m_offset, targetRange.m_size);

  // TODO(vasumahesh1): Should Amalgamate Free Ranges?
}
} // namespace Memory
} // namespace Azura
