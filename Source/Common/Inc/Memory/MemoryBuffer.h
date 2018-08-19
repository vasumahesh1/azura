#pragma once

#include "Types.h"

namespace Azura {
namespace Memory {
class MemoryBuffer {

public:
  MemoryBuffer() = default;
  explicit MemoryBuffer(U32 size);
  MemoryBuffer(U32 size, AddressPtr blockStart);
  virtual ~MemoryBuffer() = default;

  MemoryBuffer(const MemoryBuffer& other) = delete;
  MemoryBuffer& operator=(const MemoryBuffer& other) = delete;

  MemoryBuffer(MemoryBuffer&& other) noexcept;
  MemoryBuffer& operator=(MemoryBuffer&& other) noexcept;

  virtual void* Allocate(U32 size) = 0;
  virtual void Deallocate(void* address) = 0;

#ifdef BUILD_UNIT_TEST
    AddressPtr GetCurrentPtr() const { return m_currentPosition; };
    AddressPtr GetBasePtr() const { return m_memoryBlock; };
    U32 GetSize() const { return m_size; };
#endif

protected:
  AddressPtr m_memoryBlock{0};
  AddressPtr m_currentPosition{0};
  U32 m_size{0};
};
} // namespace Memory
} // namespace Azura
