#include "MemoryAllocators/MemoryBuffer.h"
#include <cassert>

namespace AZ {
  MemoryBuffer::MemoryBuffer(SizeType size)
    : mSize(size) {}

  MemoryBuffer::MemoryBuffer(SizeType size, AddressPtr blockStart)
    : mSize(size),
      mMemoryBlock(blockStart),
      mCurrentPosition(blockStart) {}

  MemoryBuffer::MemoryBuffer(MemoryBuffer&& other) noexcept
    : mMemoryBlock(other.mMemoryBlock),
      mCurrentPosition(other.mCurrentPosition),
      mSize(other.mSize) {
    other.mMemoryBlock     = 0;
    other.mCurrentPosition = 0;
    other.mSize            = 0;
  }

  MemoryBuffer& MemoryBuffer::operator=(MemoryBuffer&& other) noexcept {
    assert(this != &other);

    mMemoryBlock     = other.mMemoryBlock;
    mCurrentPosition = other.mCurrentPosition;
    mSize            = other.mSize;

    other.mMemoryBlock     = 0;
    other.mCurrentPosition = 0;
    other.mSize            = 0;

    return *this;
  }
} // namespace AZ
