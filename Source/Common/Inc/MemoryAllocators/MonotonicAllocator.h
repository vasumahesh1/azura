#pragma once

#include "Types.h"
#include "Allocator.h"
#include <cstdlib>
#include <cassert>

#include "Utils/Macros.h"


namespace Azura {
  class MonotonicAllocator : public Allocator {
    using AddressPtr = UPTR;
    using AddressDiff = ptrdiff_t;

  public:
    explicit MonotonicAllocator(SizeType size);
    MonotonicAllocator(SizeType size, void* managedResource);
    ~MonotonicAllocator() override;

    /**
     * \brief Creates a Monotonic allocator from the provided MemoryBuffer.
     */
    static MonotonicAllocator Create(SizeType size, MemoryBuffer& buffer);

    // Disable Copy
    MonotonicAllocator(const MonotonicAllocator& other) = delete;
    MonotonicAllocator& operator=(const MonotonicAllocator& other) = delete;

    // Enable Move
    MonotonicAllocator(MonotonicAllocator&& other) noexcept = default;
    MonotonicAllocator& operator=(MonotonicAllocator&& other) noexcept = default;

    /**
     * \brief Allocates a set number of bytes
     * \param size Number of Bytes to Allocate
     * \param alignment Alignment for the data
     * \return Pointer to Memory
     */
    void* Allocate(SizeType size, SizeType alignment) override;

    /**
     * \brief Currently a No Op.
     * \param address Address to Deallocate
     */
    void Deallocate(void* address) override;

    /**
    * \brief Reset the Memory for fresh new use.
    */
    void Reset() override;

#ifdef TOOLS_TEST
    AddressPtr GetCurrentPtr() const { return mCurrentPosition; };
    AddressPtr GetBasePtr() const { return mMemoryBlock; };
    AddressPtr GetSize() const { return mSize; };
#endif

  private:
    SizeType mSize;
    AddressPtr mMemoryBlock;
    AddressPtr mCurrentPosition;
    bool mIsManagedResource{ false };
  };
} // namespace AZ
