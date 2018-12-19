
.. _program_listing_file_Source_Containers_Benchmark_StdVector_benchmark.h:

Program Listing for File StdVector_benchmark.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Containers_Benchmark_StdVector_benchmark.h>` (``Source\Containers\Benchmark\StdVector_benchmark.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include <benchmark/benchmark.h>
   #include <vector>
   
   class VectorBenchmarkFixture : public benchmark::Fixture
   {
   public:
     VectorBenchmarkFixture() {}
   
     void TearDown(const benchmark::State&) override
     {}
   };
