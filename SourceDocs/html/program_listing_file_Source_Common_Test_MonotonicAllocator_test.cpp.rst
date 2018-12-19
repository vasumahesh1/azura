
.. _program_listing_file_Source_Common_Test_MonotonicAllocator_test.cpp:

Program Listing for File MonotonicAllocator_test.cpp
====================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Test_MonotonicAllocator_test.cpp>` (``Source\Common\Test\MonotonicAllocator_test.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include <iostream>
   #include <limits>
   
   #include "MonotonicAllocator_test.h"
   
   struct Sample {
     int arg1;
     double arg2;
   
     Sample() {
       arg1 = 99;
       arg2 = 16.0;
     }
   };
   
   TEST(MonotonicAllocatorTest, Constructs) {
     Azura::Memory::HeapMemoryBuffer myBuffer = Azura::Memory::HeapMemoryBuffer(256);
     Azura::Memory::MonotonicAllocator target = Azura::Memory::MonotonicAllocator(myBuffer, 128);
   }
   
   TEST(MonotonicAllocatorTest, AllocateNormalData) {
     Azura::Memory::HeapMemoryBuffer myBuffer = Azura::Memory::HeapMemoryBuffer(256);
     Azura::Memory::MonotonicAllocator target = Azura::Memory::MonotonicAllocator(myBuffer, 128);
   
     auto ptr = target.New<int>(0);
   
     ASSERT_NE(ptr, nullptr);
     ASSERT_EQ(*ptr, 0);
   
     *ptr = 99;
   
     ASSERT_EQ(*ptr, 99.0);
   }
   
   TEST(MonotonicAllocatorTest, AllocateStructData) {
     Azura::Memory::HeapMemoryBuffer myBuffer = Azura::Memory::HeapMemoryBuffer(256);
     Azura::Memory::MonotonicAllocator target = Azura::Memory::MonotonicAllocator(myBuffer, 128);
   
     auto ptr = target.New<Sample>();
   
     ASSERT_NE(ptr, nullptr);
     ASSERT_EQ(ptr->arg1, 99);
     ASSERT_EQ(ptr->arg2, 16.0);
   }
   
   TEST(MonotonicAllocatorTest, AllocateNormalData2) {
     Azura::Memory::HeapMemoryBuffer myBuffer = Azura::Memory::HeapMemoryBuffer(256);
     Azura::Memory::MonotonicAllocator target = Azura::Memory::MonotonicAllocator(myBuffer, 128);
   
     uintptr_t base = target.GetBasePtr();
   
     auto ptr   = target.New<Sample>();
     auto ptr2  = target.New<Sample>();
     ptr2->arg1 = 12;
     ptr2->arg2 = 7.0;
   
     ASSERT_EQ(base, (uintptr_t)ptr.get());
     ASSERT_EQ(base + sizeof(Sample), (uintptr_t)ptr2.get());
   
     ASSERT_NE(ptr, nullptr);
     ASSERT_EQ(ptr->arg1, 99);
     ASSERT_EQ(ptr->arg2, 16.0);
   
     ASSERT_NE(ptr2, nullptr);
     ASSERT_EQ(ptr2->arg1, 12);
     ASSERT_EQ(ptr2->arg2, 7.0);
   }
