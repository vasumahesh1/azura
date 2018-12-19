
.. _program_listing_file_Source_Common_Inc_Memory_MonotonicAllocator.h:

Program Listing for File MonotonicAllocator.h
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Inc_Memory_MonotonicAllocator.h>` (``Source\Common\Inc\Memory\MonotonicAllocator.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "Allocator.h"
   #include "Types.h"
   
   namespace Azura {
   namespace Memory {
   class MemoryBuffer;
   
   class MonotonicAllocator final : public Allocator {
    public:
     MonotonicAllocator(MemoryBuffer& buffer, U32 size);
     ~MonotonicAllocator() override;
   
     MonotonicAllocator(const MonotonicAllocator& other) = delete;
     MonotonicAllocator& operator=(const MonotonicAllocator& other) = delete;
   
     MonotonicAllocator(MonotonicAllocator&& other) noexcept = default;
     MonotonicAllocator& operator=(MonotonicAllocator&& other) noexcept = delete;
   
   #ifdef BUILD_UNIT_TEST
     AddressPtr GetCurrentPtr() const {
       return m_headPtr;
     };
   #endif
   
     void Reset() override;
   
    protected:
     void* Allocate(U32 size, U32 alignment) override;
   
     void Deallocate(void* address) override;
   
    private:
     AddressPtr m_headPtr;
     MemoryBuffer& m_sourceBuffer;
   };
   }  // namespace Memory
   }  // namespace Azura
