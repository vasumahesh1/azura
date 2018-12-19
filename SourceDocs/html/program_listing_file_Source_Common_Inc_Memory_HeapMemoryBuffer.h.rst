
.. _program_listing_file_Source_Common_Inc_Memory_HeapMemoryBuffer.h:

Program Listing for File HeapMemoryBuffer.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Inc_Memory_HeapMemoryBuffer.h>` (``Source\Common\Inc\Memory\HeapMemoryBuffer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "MemoryBuffer.h"
   #include "Types.h"
   
   namespace Azura {
   namespace Memory {
   class HeapMemoryBuffer final : public MemoryBuffer {
     using Byte = U8;
   
    public:
     explicit HeapMemoryBuffer(U32 blockSize);
   
     HeapMemoryBuffer(U32 blockSize, U32 alignment);
   
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
