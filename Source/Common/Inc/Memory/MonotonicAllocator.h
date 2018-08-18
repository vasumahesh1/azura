#pragma once

#include "Types.h"
#include "Allocator.h"

namespace Azura {
namespace Memory {
class MonotonicAllocator final : public Allocator {
public:
  MonotonicAllocator(MemoryBuffer& buffer, U32 size);
  ~MonotonicAllocator() override;

  MonotonicAllocator(const MonotonicAllocator& other) = delete;
  MonotonicAllocator& operator=(const MonotonicAllocator& other) = delete;

  MonotonicAllocator(MonotonicAllocator&& other) noexcept = default;
  MonotonicAllocator& operator=(MonotonicAllocator&& other) noexcept = delete;

#ifdef BUILD_UNIT_TEST
    AddressPtr GetCurrentPtr() const { return m_headPtr; };
#endif

  /**
  * \brief Reset the Memory for fresh new use.
  */
  void Reset() override;

protected:
  /**
  * \brief Allocates a set number of bytes
  * \param size Number of Bytes to Allocate
  * \param alignment Alignment for the data
  * \return Pointer to Memory
  */
  void* Allocate(U32 size, U32 alignment) override;

  /**
  * \brief Currently a No Op.
  * \param address Address to Deallocate
  */
  void Deallocate(void* address) override;

private:
  AddressPtr m_headPtr;
  MemoryBuffer& m_sourceBuffer;
};
} // namespace Memory
} // namespace Azura
