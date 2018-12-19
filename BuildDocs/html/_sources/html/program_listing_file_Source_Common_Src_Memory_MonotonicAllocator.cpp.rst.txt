
.. _program_listing_file_Source_Common_Src_Memory_MonotonicAllocator.cpp:

Program Listing for File MonotonicAllocator.cpp
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Src_Memory_MonotonicAllocator.cpp>` (``Source\Common\Src\Memory\MonotonicAllocator.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   
   
   #include "Memory/MonotonicAllocator.h"
   #include <cassert>
   #include "Memory/MemoryBuffer.h"
   #include "Utils/Macros.h"
   
   namespace Azura {
   namespace Memory {
   
   namespace {
   SizeType AlignAhead(SizeType size, SizeType alignment) {
     return (size + (alignment - 1)) & ~(alignment - 1);
   }
   } // namespace
   
   MonotonicAllocator::MonotonicAllocator(MemoryBuffer& buffer, U32 size)
     : Allocator(buffer.Allocate(size), size),
       m_headPtr(BasePtr()),
       m_sourceBuffer(buffer) {
   }
   
   MonotonicAllocator::~MonotonicAllocator() {
     // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
     m_sourceBuffer.Deallocate(reinterpret_cast<void*>(BasePtr()));
   }
   
   void* MonotonicAllocator::Allocate(U32 size, U32 alignment) {
     // Power of 2 check
     assert((alignment & (alignment - 1)) == 0);
   
     const SizeType alignedSize = AlignAhead(size, alignment);
   
     if (Size() - (m_headPtr - BasePtr()) < alignedSize)
     {
       return nullptr;
     }
   
     const SizeType mask     = alignment - 1;
     const UPTR misalignment = (m_headPtr & mask);
   
     // TODO(vasumahesh1): Need to figure this out.
     const AddressDiff adjustment = [&]() -> AddressDiff
     {
       if (misalignment > 0) {
         return alignment - misalignment;
       }
   
       return AddressPtr(0);
     }();
   
     const UPTR addr = m_headPtr + adjustment;
     m_headPtr       = addr + alignedSize;
   
     // TODO(vasumahesh1): Store adjusment appropriately for deallocation
     // assert(adjustment < 256);
     // Byte* pAlignedMem = reinterpret_cast<Byte*>(alignedAddress);
     // pAlignedMem[-1] = static_cast<Byte>(adjustment);
   
     // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
     return reinterpret_cast<void*>(addr);
   }
   
   void MonotonicAllocator::Deallocate(void* address) {
     UNUSED(address);
   }
   
   void MonotonicAllocator::Reset() {
     m_headPtr = BasePtr();
   }
   
   } // namespace Memory
   } // namespace Azura
