
.. _program_listing_file_Source_Containers_Test_Array_test.h:

Program Listing for File Array_test.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Containers_Test_Array_test.h>` (``Source\Containers\Test\Array_test.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Containers/Array.h"
   #include "Memory/HeapMemoryBuffer.h"
   #include "Memory/MonotonicAllocator.h"
   #include "gtest/gtest.h"
   
   using namespace Azura::Containers;
   using namespace Azura::Memory;
   
   class ArrayTest : public ::testing::Test {
    protected:
     ArrayTest() : buffer(Azura::Memory::HeapMemoryBuffer(1024)), alloc(Azura::Memory::MonotonicAllocator(buffer, 1024)) {}
   
     virtual ~ArrayTest() {}
   
     Azura::Memory::HeapMemoryBuffer buffer;
     Azura::Memory::MonotonicAllocator alloc;
   };
