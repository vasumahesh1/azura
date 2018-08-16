#pragma once

#include "Types.h"

namespace Azura {
namespace Memory {
class MemoryBuffer {

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
    AddressPtr GetCurrentPtr() const { return m_headPtr; };
    AddressPtr GetBasePtr() const { return m_memoryBlock; };
    AddressPtr GetSize() const { return m_size; };
#endif

protected:
  AddressPtr m_memoryBlock{0};
  AddressPtr m_currentPosition{0};
  SizeType mSize{0};
};
} // namespace Memory
} // namespace Azura
