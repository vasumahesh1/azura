#pragma once

#include "Types.h"
#include "Allocator.h"

#include <vector>

namespace Azura {
namespace Memory {
class RangeAllocator final : public Allocator {
public:
  RangeAllocator(MemoryBuffer& buffer, U32 size);
  ~RangeAllocator();

  // Disable Copy
  RangeAllocator(const RangeAllocator& other) = delete;
  RangeAllocator& operator=(const RangeAllocator& other) = delete;

  // Enable Move
  RangeAllocator(RangeAllocator&& other) noexcept = default;
  RangeAllocator& operator=(RangeAllocator&& other) noexcept = delete;

  void* Allocate(U32 size, U32 alignment) override;
  void Deallocate(void* address) override;
private:
  MemoryBuffer& m_sourceBuffer;

  // TODO(vasumahesh1): Needs a performance boost here
  std::vector<MemoryRange> m_freeRanges{1, MemoryRange(0, Size())};
  std::vector<MemoryRange> m_occupiedRanges;
};
} // namespace Memory
} // namespace Azura
