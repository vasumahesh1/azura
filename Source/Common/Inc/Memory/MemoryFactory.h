#pragma once
#include "Types.h"


namespace Azura
{
  #define STACK_ALLOCATOR(NAME, TYPE, SIZE)                                                           \
    Memory::StackMemoryBuffer<SIZE> buffer ## NAME;                                            \
    TYPE allocator ## NAME = TYPE(buffer ## NAME, SIZE);
}
