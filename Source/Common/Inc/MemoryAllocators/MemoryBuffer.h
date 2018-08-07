#pragma once

#include "Types.h"

namespace AZ {
  class MemoryBuffer {
    using AddressPtr = UPTR;

  public:
    MemoryBuffer() = default;
    explicit MemoryBuffer(SizeType size);
    MemoryBuffer(SizeType size, AddressPtr blockStart);
    virtual ~MemoryBuffer() = default;

    MemoryBuffer(const MemoryBuffer& other) = delete;
    MemoryBuffer& operator=(const MemoryBuffer& other) = delete;

    MemoryBuffer(MemoryBuffer&& other) noexcept;
    MemoryBuffer& operator=(MemoryBuffer&& other) noexcept;

    virtual void* Allocate(SizeType size) = 0;
    virtual void Deallocate(void* address) = 0;

#ifdef TOOLS_TEST
    AddressPtr GetCurrentPtr() const { return mCurrentPosition; };
    AddressPtr GetBasePtr() const { return mMemoryBlock; };
    AddressPtr GetSize() const { return mSize; };
#endif

  protected:
    AddressPtr mMemoryBlock{0};
    AddressPtr mCurrentPosition{0};
    SizeType mSize{0};
  };
} // namespace AZ
