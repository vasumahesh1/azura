#pragma once

#include "Types.h"
#include "MemoryBuffer.h"

namespace Azura {
namespace Memory {
class HeapMemoryBuffer : public MemoryBuffer {
  using Byte = U8;

public:
  /**
   * \brief Allocate a Block on the Heap for Usage
   * \param blockSize Size of Memory Block to allocate
   */
  explicit HeapMemoryBuffer(SizeType blockSize);

  /**
   * \brief Allocate a Aligned Block on the Heap for Usage
   * \param blockSize Size of Memory Block to allocate
   * \param alignment Starting Alignment for Block
   */
  HeapMemoryBuffer(SizeType blockSize, SizeType alignment);

  /**
   * \brief Destroys the Heap Allocated Memory
   */
  ~HeapMemoryBuffer();

  // Disabled Copy Ctor
  HeapMemoryBuffer(const HeapMemoryBuffer& other) = delete;
  // Disabled Copy Assignment
  HeapMemoryBuffer& operator=(const HeapMemoryBuffer& other) = delete;

  HeapMemoryBuffer(HeapMemoryBuffer&& other) noexcept = default;
  HeapMemoryBuffer& operator=(HeapMemoryBuffer&& other) noexcept = default;

  void* Allocate(SizeType size) override;
  void Deallocate(void* address) override;

private:
  UPTR AllocateRaw(SizeType size);
};
} // namespace Memory
} // namespace Azura
