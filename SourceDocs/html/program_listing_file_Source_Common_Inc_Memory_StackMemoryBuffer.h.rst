
.. _program_listing_file_Source_Common_Inc_Memory_StackMemoryBuffer.h:

Program Listing for File StackMemoryBuffer.h
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Inc_Memory_StackMemoryBuffer.h>` (``Source\Common\Inc\Memory\StackMemoryBuffer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <cassert>
   
   #include "MemoryBuffer.h"
   #include "Types.h"
   #include "Utils/Macros.h"
   
   namespace Azura {
   namespace Memory {
   
   template <U32 Size>
   class StackMemoryBuffer final : public MemoryBuffer {
    public:
     StackMemoryBuffer();
     ~StackMemoryBuffer() = default;
   
     // Disabled Copy Ctor
     StackMemoryBuffer(const StackMemoryBuffer& other) = delete;
     // Disabled Copy Assignment
     StackMemoryBuffer& operator=(const StackMemoryBuffer& other) = delete;
   
     StackMemoryBuffer(StackMemoryBuffer&& other) noexcept = default;
     StackMemoryBuffer& operator=(StackMemoryBuffer&& other) noexcept = default;
   
     void* Allocate(U32 size) override;
     void Deallocate(void* address) override;
   
    private:
     UPTR AllocateRaw(U32 size);
     U8 m_buffer[Size];
   };
   
   template <U32 Size>
   StackMemoryBuffer<Size>::StackMemoryBuffer() : MemoryBuffer(Size, reinterpret_cast<AddressPtr>(m_buffer)), m_buffer{} {}
   
   template <U32 Size>
   void* StackMemoryBuffer<Size>::Allocate(U32 size) {
     const AddressPtr addr = AllocateRaw(size);
   
     // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
     return reinterpret_cast<void*>(addr);
   }
   
   template <U32 Size>
   void StackMemoryBuffer<Size>::Deallocate(void* address) {
     UNUSED(address);
   }
   
   template <U32 Size>
   UPTR StackMemoryBuffer<Size>::AllocateRaw(U32 size) {
     // Available < Size, Can't allocate
     if (m_size - (m_currentPosition - m_memoryBlock) < size) {
       return 0u;
     }
   
     const AddressPtr addr = m_currentPosition;
     m_currentPosition += size;
   
     return addr;
   }
   }  // namespace Memory
   }  // namespace Azura
