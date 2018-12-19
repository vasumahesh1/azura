
.. _program_listing_file_Source_PowerBench_Src_MatrixMultiplication.cpp:

Program Listing for File MatrixMultiplication.cpp
=================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_PowerBench_Src_MatrixMultiplication.cpp>` (``Source\PowerBench\Src\MatrixMultiplication.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "PowerBench/MatrixMultiplication.h"
   
   using namespace Utils;
   
   BENCHMARK_DEFINE_F(MatrixMultiplicationFixture, BM_MMNaive)(benchmark::State& st) {
     for (auto _ : st) {
       st.PauseTiming();
       Utils::ClearCache();
       st.ResumeTiming();
   
       const auto size = st.range(0);
   
       for (int i = 0; i < size; ++i)
       {
         for (int j = 0; j < size; ++j)
         {
           for (int k = 0; k < size; ++k)
           {
             m_result[i * size + j] += m_data1[i * size + k] * m_data2[k * size + j];
           }
         }
       }
     }
   }
   BENCHMARK_REGISTER_F(MatrixMultiplicationFixture, BM_MMNaive)->RangeMultiplier(2)->Range(256, 256 << 3);
   
   
   BENCHMARK_DEFINE_F(MatrixMultiplicationFixture, BM_MMCacheOptimized)(benchmark::State& st) {
     for (auto _ : st) {
       st.PauseTiming();
       Utils::ClearCache();
       st.ResumeTiming();
   
       const auto size = st.range(0);
   
       double* rres;
       double* rmul1;
       double* rmul2;
   
       uint32_t i2, j2, k2;
   
       for (uint32_t i = 0; i < size; i += ELEMENTS_PER_LINE)
       {
         for (uint32_t j = 0; j < size; j += ELEMENTS_PER_LINE)
         {
           for (uint32_t k = 0; k < size; k += ELEMENTS_PER_LINE)
           {
             for (i2 = 0, rres = &m_result[i * size + j], rmul1 = &m_data1[i * size + k]; i2 < ELEMENTS_PER_LINE; ++i2, rres += size, rmul1 += size)
             {
               for (k2 = 0, rmul2 = &m_data2[k * size + j]; k2 < ELEMENTS_PER_LINE; ++k2, rmul2 += size)
               {
                 for (j2 = 0; j2 < ELEMENTS_PER_LINE; ++j2)
                 {
                   rres[j2] += rmul1[k2] * rmul2[j2];
                 }
               }
             }
           }
         }
       }
     }
   }
   BENCHMARK_REGISTER_F(MatrixMultiplicationFixture, BM_MMCacheOptimized)->RangeMultiplier(2)->Range(256, 256 << 3);
