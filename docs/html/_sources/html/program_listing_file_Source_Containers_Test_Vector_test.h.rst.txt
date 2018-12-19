
.. _program_listing_file_Source_Containers_Test_Vector_test.h:

Program Listing for File Vector_test.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Containers_Test_Vector_test.h>` (``Source\Containers\Test\Vector_test.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Containers/Vector.h"
   #include "Memory/HeapMemoryBuffer.h"
   #include "Memory/MonotonicAllocator.h"
   #include "gtest/gtest.h"
   
   using namespace Azura::Containers;
   using namespace Azura::Memory;
   
   class VectorTest : public ::testing::Test {
    protected:
     VectorTest()
         : buffer(Azura::Memory::HeapMemoryBuffer(1024)), alloc(Azura::Memory::MonotonicAllocator(buffer, 1024)) {}
   
     virtual ~VectorTest() {}
   
     Azura::Memory::HeapMemoryBuffer buffer;
     Azura::Memory::MonotonicAllocator alloc;
   };
