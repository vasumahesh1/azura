#pragma once
#include "Memory/StackMemoryBuffer.h"
#include "Memory/HeapMemoryBuffer.h"
#include "Types.h"

namespace Azura {
#define STACK_ALLOCATOR(NAME, TYPE, SIZE)                                                                              \
  Memory::StackMemoryBuffer<SIZE> buffer##NAME;                                                                        \
  TYPE allocator##NAME = TYPE(buffer##NAME, SIZE);

#define HEAP_ALLOCATOR(NAME, TYPE, SIZE)                                                                              \
  Memory::HeapMemoryBuffer buffer##NAME = Memory::HeapMemoryBuffer(SIZE);                                                     \
  TYPE allocator##NAME = TYPE(buffer##NAME, SIZE);
}  // namespace Azura
