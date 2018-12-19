
.. _program_listing_file_Source_Common_Src_Memory_StackAllocator.cpp:

Program Listing for File StackAllocator.cpp
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Src_Memory_StackAllocator.cpp>` (``Source\Common\Src\Memory\StackAllocator.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Memory/StackAllocator.h"
   #include <stdlib.h>
   
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
   
   }  // namespace AZ
