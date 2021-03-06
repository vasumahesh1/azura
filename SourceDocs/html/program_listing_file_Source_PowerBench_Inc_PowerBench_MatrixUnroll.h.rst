
.. _program_listing_file_Source_PowerBench_Inc_PowerBench_MatrixUnroll.h:

Program Listing for File MatrixUnroll.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_PowerBench_Inc_PowerBench_MatrixUnroll.h>` (``Source\PowerBench\Inc\PowerBench\MatrixUnroll.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <benchmark/benchmark.h>
   
   #include "PowerBench/Utils.h"
   
   class MatrixUnrollFixture : public benchmark::Fixture
   {
   public:
     void TearDown(const benchmark::State&) override
     {
       delete[] m_data1;
       delete[] m_data2;
       delete[] m_result;
     }
   
     void SetUp(const benchmark::State& st) override
     {
       const auto fixtureSize = st.range(0);
   
       m_data1 = new double[fixtureSize * fixtureSize];
       Utils::CreateRandomMatrix(fixtureSize, m_data1);
   
       m_result = new double[fixtureSize * fixtureSize];
       Utils::CreateEmpty(fixtureSize, m_result);
     }
   
     double* m_data1;
     double* m_data2;
     double* m_result;
   };
