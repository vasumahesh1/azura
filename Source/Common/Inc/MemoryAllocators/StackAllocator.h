#pragma once

#include "Types.h"

namespace Azura
{
// Stack Allocator based on a header file template provided in
// Game Engine Architecture - Jason Gregory
class StackAllocator
{
public:
  using Marker = UPTR;

  // Allocates a pool of memory for the stack to use
  explicit StackAllocator(U32 stackSizeInBytes);

  // Deallocates the pool of memory which was being used by the stack
  ~StackAllocator();

  // Allocates a block a memory as long as it can fit in the stack
  // else returns nullptr
  void* Allocate(U32 bytesToAllocate);

  // Frees memory till a marker. use getMarker() to get the memory pointer
  // and later free to it when needed
  void FreeToMarker(Marker marker);

  // Get the current memory pointer in the stack
  Marker GetMarker() const;

  // Empty the stack memory
  void Clear();

private:
  Marker mCurrentMarker;
  Marker mBaseMarker;
  U32 mSize;
};
} // namespace Azura
