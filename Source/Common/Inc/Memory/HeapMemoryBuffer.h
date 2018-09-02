#pragma once

#include "MemoryBuffer.h"
#include "Types.h"

namespace Azura {
namespace Memory {
class HeapMemoryBuffer final : public MemoryBuffer {
  using Byte = U8;

 public:
  /**
   * \brief Allocate a Block on the Heap for Usage
   * \param blockSize Size of Memory Block to allocate
   */
  explicit HeapMemoryBuffer(U32 blockSize);

  /**
   * \brief Allocate a Aligned Block on the Heap for Usage
   * \param blockSize Size of Memory Block to allocate
   * \param alignment Starting Alignment for Block
   */
  HeapMemoryBuffer(U32 blockSize, U32 alignment);

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

  void* Allocate(U32 size) override;
  void Deallocate(void* address) override;

 private:
  UPTR AllocateRaw(U32 size);
  bool m_isAllocationAligned;
};
}  // namespace Memory
}  // namespace Azura
