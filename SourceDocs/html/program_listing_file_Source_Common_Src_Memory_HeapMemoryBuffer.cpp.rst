
.. _program_listing_file_Source_Common_Src_Memory_HeapMemoryBuffer.cpp:

Program Listing for File HeapMemoryBuffer.cpp
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Src_Memory_HeapMemoryBuffer.cpp>` (``Source\Common\Src\Memory\HeapMemoryBuffer.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Memory/HeapMemoryBuffer.h"
   #include <cassert>
   #include <cstdlib>
   
   #include "Utils/Macros.h"
   
   namespace Azura {
   namespace Memory {
   HeapMemoryBuffer::HeapMemoryBuffer(const U32 blockSize)
       // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, cppcoreguidelines-no-malloc)
       : MemoryBuffer(blockSize, reinterpret_cast<AddressPtr>(malloc(blockSize))), m_isAllocationAligned(false) {}
   
   HeapMemoryBuffer::HeapMemoryBuffer(U32 blockSize, U32 alignment)
       : MemoryBuffer(blockSize,
                      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, cppcoreguidelines-no-malloc)
                      reinterpret_cast<AddressPtr>(ALIGNED_ALLOC(alignment, blockSize))),
         m_isAllocationAligned(true) {}
   
   HeapMemoryBuffer::~HeapMemoryBuffer() {
     if (m_memoryBlock == 0u) {
       return;
     }
   
     if (m_isAllocationAligned) {
       // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-reinterpret-cast)
       ALIGNED_FREE(reinterpret_cast<void*>(m_memoryBlock));  // NOLINT(cppcoreguidelines-no-malloc)
     }
     else {
       // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-reinterpret-cast)
       free(reinterpret_cast<void*>(m_memoryBlock));  // NOLINT(cppcoreguidelines-no-malloc)
     }
   }
   
   void* HeapMemoryBuffer::Allocate(U32 size) {
     const AddressPtr addr = AllocateRaw(size);
   
     // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
     return reinterpret_cast<void*>(addr);
   }
   
   void HeapMemoryBuffer::Deallocate(void* address) {
     UNUSED(address);
   }
   
   UPTR HeapMemoryBuffer::AllocateRaw(U32 size) {
     // Available > Size
     assert(m_size - (m_currentPosition - m_memoryBlock) >= size);
   
     const AddressPtr addr = m_currentPosition;
     m_currentPosition += size;
   
     return addr;
   }
   }  // namespace Memory
   }  // namespace Azura
