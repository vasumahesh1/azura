#include <stdlib.h>
#include "Memory/StackAllocator.h"

namespace AZ {
  StackAllocator::StackAllocator(U32 stackSizeInBytes) {
    mCurrentMarker = (Marker)malloc(stackSizeInBytes);
    mBaseMarker    = mCurrentMarker;
    mSize          = stackSizeInBytes;
  }

  StackAllocator::~StackAllocator() {
    free((void*)mBaseMarker);
  }

  void* StackAllocator::Allocate(U32 bytesToAllocate) {
    Marker addrStart = 0;

    if (bytesToAllocate <= mSize) {
      U32 availableBytes = mSize - static_cast<U32>(mCurrentMarker - mBaseMarker);

      if (bytesToAllocate <= availableBytes) {

        addrStart = mCurrentMarker;
        mCurrentMarker += bytesToAllocate;
      }
    }

    return reinterpret_cast<void*>(addrStart);
  }

  void StackAllocator::FreeToMarker(Marker marker) {
    mCurrentMarker = marker;
  }

  StackAllocator::Marker StackAllocator::GetMarker() const {
    return mCurrentMarker;
  }

  void StackAllocator::Clear() {
    mCurrentMarker = mBaseMarker;
  }

} // namespace Azura
