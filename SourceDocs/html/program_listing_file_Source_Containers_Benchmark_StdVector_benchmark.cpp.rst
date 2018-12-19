
.. _program_listing_file_Source_Containers_Benchmark_StdVector_benchmark.cpp:

Program Listing for File StdVector_benchmark.cpp
================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Containers_Benchmark_StdVector_benchmark.cpp>` (``Source\Containers\Benchmark\StdVector_benchmark.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "StdVector_benchmark.h"
   
   #include "Types.h"
   
   using namespace Azura;             // NOLINT
   
   class ComplexType {
     int m_data;
   
   public:
     static int s_ctorCalls;
     static int s_dtorCalls;
     static int s_copyCtorCalls;
     static int s_moveCtorCalls;
     static int s_copyAssignCalls;
     static int s_moveAssignCalls;
   
     static void ResetStats() {
       s_ctorCalls       = 0;
       s_dtorCalls       = 0;
       s_copyCtorCalls   = 0;
       s_moveCtorCalls   = 0;
       s_copyAssignCalls = 0;
       s_moveAssignCalls = 0;
   
       // TODO(vasumahesh1): Test Logs needed.
       // std::cout << "Reset" << std::endl;
     }
   
     ComplexType(int data)
       : m_data(data) {
       // std::cout << "Ctor" << std::endl;
       ++s_ctorCalls;
     }
   
     ~ComplexType() {
       // std::cout << "Dtor" << std::endl;
       ++s_dtorCalls;
     }
   
     ComplexType(const ComplexType& other)
       : m_data(other.m_data) {
       // std::cout << "Copy Ctor" << std::endl;
       ++s_copyCtorCalls;
     }
   
     ComplexType(ComplexType&& other) noexcept
       : m_data(std::move(other.m_data)) {
       // std::cout << "Move Ctor" << std::endl;
       ++s_moveCtorCalls;
     }
   
     ComplexType& operator=(const ComplexType& other) {
       // std::cout << "Copy = " << std::endl;
       m_data = other.m_data;
       ++s_copyAssignCalls;
       return *this;
     }
   
     ComplexType& operator=(ComplexType&& other) noexcept {
       // std::cout << "Move = " << std::endl;
       m_data = std::move(other.m_data);
       ++s_moveAssignCalls;
       return *this;
     }
   
     int Data() const {
       return m_data;
     }
   };
   
   int ComplexType::s_ctorCalls       = 0;
   int ComplexType::s_dtorCalls       = 0;
   int ComplexType::s_copyCtorCalls   = 0;
   int ComplexType::s_moveCtorCalls   = 0;
   int ComplexType::s_copyAssignCalls = 0;
   int ComplexType::s_moveAssignCalls = 0;
   
   BENCHMARK_F(VectorBenchmarkFixture, BM_EmptyCreation)(benchmark::State& st) {
     for (auto _ : st) {
       std::vector<int> v;
     }
   }
   
   BENCHMARK_F(VectorBenchmarkFixture, BM_EmptyCreation_ComplexType)(benchmark::State& st) {
     for (auto _ : st) {
       std::vector<ComplexType> v;
     }
   }
   
   BENCHMARK_DEFINE_F(VectorBenchmarkFixture, BM_AllocateCreation)(benchmark::State& st) {
     for (auto _ : st) {
       std::vector<int> v(U32(st.range(0)));
     }
   }
   BENCHMARK_REGISTER_F(VectorBenchmarkFixture, BM_AllocateCreation)->RangeMultiplier(2)->Range(8, 8 << 10);
   
   BENCHMARK_DEFINE_F(VectorBenchmarkFixture, BM_InsertWithReserve)(benchmark::State& st) {
     for (auto _ : st) {
       std::vector<int> v;
       v.reserve(U32(st.range(0)));
   
       for (U32 i = 0; i < U32(st.range(0)); ++i)
       {
         v.push_back(i);
       }
     }
   }
   BENCHMARK_REGISTER_F(VectorBenchmarkFixture, BM_InsertWithReserve)->RangeMultiplier(2)->Range(8, 8 << 10);
   
   BENCHMARK_DEFINE_F(VectorBenchmarkFixture, BM_InsertWithReserve_Heavy)(benchmark::State& st) {
     for (auto _ : st) {
   
       for (U32 size = 0; size < U32(st.range(0)); ++size)
       {
         std::vector<int> v;
         v.reserve(size);
   
         for (U32 i = 0; i < size; ++i) {
           v.push_back(i);
         }
       }
     }
   }
   BENCHMARK_REGISTER_F(VectorBenchmarkFixture, BM_InsertWithReserve_Heavy)->RangeMultiplier(2)->Range(8, 8 << 10);
   
   BENCHMARK_DEFINE_F(VectorBenchmarkFixture, BM_SetWithResize)(benchmark::State& st) {
     for (auto _ : st) {
       std::vector<int> v(U32(st.range(0)));
   
       for (U32 i = 0; i < U32(st.range(0)); ++i)
       {
         v[i] = i;
       }
     }
   }
   BENCHMARK_REGISTER_F(VectorBenchmarkFixture, BM_SetWithResize)->RangeMultiplier(2)->Range(8, 8 << 10);
