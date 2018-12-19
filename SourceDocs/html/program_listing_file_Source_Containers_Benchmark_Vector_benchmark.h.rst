
.. _program_listing_file_Source_Containers_Benchmark_Vector_benchmark.h:

Program Listing for File Vector_benchmark.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Containers_Benchmark_Vector_benchmark.h>` (``Source\Containers\Benchmark\Vector_benchmark.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <benchmark/benchmark.h>
   
   #include "Memory/MemoryFactory.h"
   #include "Memory/MonotonicAllocator.h"
   
   class VectorBenchmarkFixture : public benchmark::Fixture
   {
   public:
     VectorBenchmarkFixture()
       : buffer(Azura::Memory::HeapMemoryBuffer(0x8000000)),
         alloc(Azura::Memory::MonotonicAllocator(buffer, 0x8000000)) {}
   
     void TearDown(const benchmark::State&) override
     {
       alloc.Reset();
     }
   
   public:
     Azura::Memory::HeapMemoryBuffer buffer;
     Azura::Memory::MonotonicAllocator alloc;
   };
