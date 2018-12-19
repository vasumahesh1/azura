
.. _program_listing_file_Source_Common_Test_RangeAllocator_test.cpp:

Program Listing for File RangeAllocator_test.cpp
================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Test_RangeAllocator_test.cpp>` (``Source\Common\Test\RangeAllocator_test.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include <iostream>
   #include <limits>
   
   #include "RangeAllocator_test.h"
   #include "Types.h"
   
   struct Sample {
     int arg1;
     double arg2;
   
     Sample() {
       arg1 = 99;
       arg2 = 16.0;
     }
   };
   
   TEST(RangeAllocatorTest, Constructs) {
     Azura::Memory::HeapMemoryBuffer myBuffer     = Azura::Memory::HeapMemoryBuffer(256);
     Azura::Memory::RangeAllocator rangeAllocator = Azura::Memory::RangeAllocator(myBuffer, 128);
   }
   
   TEST(RangeAllocatorTest, AllocateNormalData) {
     Azura::Memory::HeapMemoryBuffer myBuffer     = Azura::Memory::HeapMemoryBuffer(256);
     Azura::Memory::RangeAllocator rangeAllocator = Azura::Memory::RangeAllocator(myBuffer, 128);
   
     const auto basePtr  = rangeAllocator.GetBasePtr();
     const auto baseSize = rangeAllocator.GetSize();
   
     auto ptr = rangeAllocator.New<int>(0);
   
     ASSERT_NE(ptr, nullptr);
     ASSERT_EQ(*ptr, 0);
   
     *ptr = 99;
   
     ASSERT_EQ(*ptr, 99.0);
   
     const auto& freeRanges     = rangeAllocator.GetFreeRanges();
     const auto& occupiedRanges = rangeAllocator.GetOccupiedRanges();
   
     ASSERT_EQ(freeRanges.size(), 1u);
     ASSERT_EQ(freeRanges[0].m_offset, sizeof(int));
     ASSERT_EQ(freeRanges[0].m_size, baseSize - sizeof(int));
   
     ASSERT_EQ(occupiedRanges.size(), 1u);
     ASSERT_EQ(occupiedRanges[0].m_offset, 0u);
     ASSERT_EQ(occupiedRanges[0].m_size, sizeof(int));
   
     ASSERT_EQ(Azura::AddressPtr(ptr.get()), basePtr);
   }
   
   TEST(RangeAllocatorTest, DeallocateNormalData) {
     Azura::Memory::HeapMemoryBuffer myBuffer     = Azura::Memory::HeapMemoryBuffer(256);
     Azura::Memory::RangeAllocator rangeAllocator = Azura::Memory::RangeAllocator(myBuffer, 128);
   
     const auto basePtr  = rangeAllocator.GetBasePtr();
     const auto baseSize = rangeAllocator.GetSize();
   
     // BLock Allocate
     {
       const auto ptr = rangeAllocator.New<int>(0);
       *ptr           = 99;
     }
   
     const auto& freeRanges     = rangeAllocator.GetFreeRanges();
     const auto& occupiedRanges = rangeAllocator.GetOccupiedRanges();
   
     ASSERT_EQ(freeRanges.size(), 2u);
     ASSERT_EQ(freeRanges[0].m_offset, sizeof(int));
     ASSERT_EQ(freeRanges[0].m_size, baseSize - sizeof(int));
     ASSERT_EQ(freeRanges[1].m_offset, 0u);
     ASSERT_EQ(freeRanges[1].m_size, sizeof(int));
   
     ASSERT_EQ(occupiedRanges.size(), 0u);
   }
