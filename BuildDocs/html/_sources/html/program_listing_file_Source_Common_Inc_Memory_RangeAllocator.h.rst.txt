
.. _program_listing_file_Source_Common_Inc_Memory_RangeAllocator.h:

Program Listing for File RangeAllocator.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Inc_Memory_RangeAllocator.h>` (``Source\Common\Inc\Memory\RangeAllocator.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "Allocator.h"
   #include "Types.h"
   
   #include <vector>
   
   namespace Azura {
   namespace Memory {
   class MemoryBuffer;
   
   class RangeAllocator final : public Allocator {
    public:
     RangeAllocator(MemoryBuffer& buffer, U32 size);
     ~RangeAllocator();
   
     // Disable Copy
     RangeAllocator(const RangeAllocator& other) = delete;
     RangeAllocator& operator=(const RangeAllocator& other) = delete;
   
     // Enable Move
     RangeAllocator(RangeAllocator&& other) noexcept = default;
     RangeAllocator& operator=(RangeAllocator&& other) noexcept = delete;
   
   #ifdef BUILD_UNIT_TEST
     const std::vector<MemoryRange>& GetFreeRanges() const {
       return m_freeRanges;
     };
     const std::vector<MemoryRange>& GetOccupiedRanges() const {
       return m_occupiedRanges;
     };
   #endif
   
    protected:
     void* Allocate(U32 size, U32 alignment) override;
     void Deallocate(void* address) override;
   
    private:
     MemoryBuffer& m_sourceBuffer;
   
     // TODO(vasumahesh1): Needs a performance boost here
     std::vector<MemoryRange> m_freeRanges{1, MemoryRange(0, Size())};
     std::vector<MemoryRange> m_occupiedRanges;
   };
   }  // namespace Memory
   }  // namespace Azura
