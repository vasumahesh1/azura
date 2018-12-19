
.. _program_listing_file_Source_Common_Test_HeapMemoryBuffer_test.cpp:

Program Listing for File HeapMemoryBuffer_test.cpp
==================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Test_HeapMemoryBuffer_test.cpp>` (``Source\Common\Test\HeapMemoryBuffer_test.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "HeapMemoryBuffer_test.h"
   
   #include "Memory/HeapMemoryBuffer.h"
   
   using namespace Azura::Memory;
   
   TEST(HeapMemoryBufferTest, Constructs) {
     HeapMemoryBuffer target{32};
   
     ASSERT_NE(target.GetCurrentPtr(), 0u);
     ASSERT_NE(target.GetBasePtr(), 0u);
     ASSERT_EQ(target.GetSize(), 32u);
   }
   
   TEST(HeapMemoryBufferTest, AlignedConstruction) {
     Azura::U32 alignment = 16;
   
     HeapMemoryBuffer target{32, alignment};
   
     ASSERT_NE(target.GetCurrentPtr(), 0u);
     ASSERT_NE(target.GetBasePtr(), 0u);
     ASSERT_EQ(target.GetSize(), 32u);
   
     Azura::U32 mask = alignment - 1;
   
     ASSERT_EQ((target.GetCurrentPtr() & mask), 0u);
   }
   
   TEST(HeapMemoryBufferTest, AlignedConstruction32) {
     Azura::U32 alignment = 32;
   
     HeapMemoryBuffer target{128, alignment};
   
     ASSERT_NE(target.GetCurrentPtr(), 0u);
     ASSERT_NE(target.GetBasePtr(), 0u);
     ASSERT_EQ(target.GetSize(), 128u);
   
     Azura::U32 mask = alignment - 1;
   
     ASSERT_EQ((target.GetCurrentPtr() & mask), 0u);
   }
   
   TEST(HeapMemoryBufferTest, AlignedConstruction256) {
     Azura::U32 alignment = 256;
   
     HeapMemoryBuffer target{128, alignment};
   
     ASSERT_NE(target.GetCurrentPtr(), 0u);
     ASSERT_NE(target.GetBasePtr(), 0u);
     ASSERT_EQ(target.GetSize(), 128u);
   
     Azura::U32 mask = alignment - 1;
   
     ASSERT_EQ((target.GetCurrentPtr() & mask), 0u);
   }
   
   TEST(HeapMemoryBufferTest, AllocateNormal) {
     Azura::U32 alignment      = 16;
     Azura::U32 allocateAmount = 32;
   
     HeapMemoryBuffer target{128, alignment};
   
     auto p1 = target.GetCurrentPtr();
   
     auto p2 = target.Allocate(allocateAmount);
   
     auto p3 = target.GetCurrentPtr();
   
     ASSERT_NE(p2, nullptr);
     ASSERT_EQ(p3 - p1, allocateAmount);
   }
