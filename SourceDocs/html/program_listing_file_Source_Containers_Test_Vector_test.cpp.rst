
.. _program_listing_file_Source_Containers_Test_Vector_test.cpp:

Program Listing for File Vector_test.cpp
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Containers_Test_Vector_test.cpp>` (``Source\Containers\Test\Vector_test.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Vector_test.h"
   #include <algorithm>
   #include <iostream>
   #include <vector>
   #include "Utils/Macros.h"
   
   using namespace Azura;
   using namespace Azura::Containers;
   
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
   
     static void PrintStats() {
       std::cout << "s_ctorCalls: " << s_ctorCalls << '\n';
       std::cout << "s_dtorCalls: " << s_dtorCalls << '\n';
       std::cout << "s_copyCtorCalls: " << s_copyCtorCalls << '\n';
       std::cout << "s_moveCtorCalls: " << s_moveCtorCalls << '\n';
       std::cout << "s_copyAssignCalls: " << s_copyAssignCalls << '\n';
       std::cout << "s_moveAssignCalls: " << s_moveAssignCalls << '\n';
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
   
   Vector<int> GetTrivialVector(Memory::Allocator& alloc) {
     Vector<int> v{16, alloc};
   
     v.PushBack(10);
     v.PushBack(20);
     v.PushBack(30);
     v.PushBack(40);
   
     return v;
   }
   
   Vector<ComplexType> GetComplexVector(Memory::Allocator& alloc) {
     Vector<ComplexType> v{16, alloc};
   
     v.EmplaceBack(10);
     v.EmplaceBack(20);
     v.EmplaceBack(30);
     v.EmplaceBack(40);
   
     return v;
   }
   
   std::vector<ComplexType> GetComplexStdVector() {
     std::vector<ComplexType> v;
     v.reserve(16);
   
     v.emplace_back(10);
     v.emplace_back(20);
     v.emplace_back(30);
     v.emplace_back(40);
   
     return v;
   }
   
   TEST_F(VectorTest, Constructs) {
     Vector<int> v{16, alloc};
     ASSERT_EQ(v.GetSize(), 0u);
   }
   
   TEST_F(VectorTest, InitializerConstruct) {
     alloc.Reset();
     Vector<int> v{{10, 20, 30, 40, 50}, alloc};
   
     ASSERT_EQ(v.GetSize(), 5u);
     ASSERT_EQ(v[0], 10);
     ASSERT_EQ(v[1], 20);
     ASSERT_EQ(v[2], 30);
     ASSERT_EQ(v[3], 40);
     ASSERT_EQ(v[4], 50);
   }
   
   TEST_F(VectorTest, NormalPush) {
     alloc.Reset();
     Vector<int> v{16, alloc};
   
     v.PushBack(10);
     v.PushBack(20);
     v.PushBack(30);
   
     ASSERT_EQ(v.GetSize(), 3u);
     ASSERT_EQ(v[0], 10);
     ASSERT_EQ(v[1], 20);
     ASSERT_EQ(v[2], 30);
   }
   
   TEST_F(VectorTest, GrowPush) {
     alloc.Reset();
     Vector<int> v{2, alloc};
   
     v.PushBack(10);
     v.PushBack(20);
     v.PushBack(30);
   
     ASSERT_EQ(v.GetSize(), 3u);
     ASSERT_EQ(v.GetMaxSize(), 4u);
     ASSERT_EQ(v[0], 10);
     ASSERT_EQ(v[1], 20);
     ASSERT_EQ(v[2], 30);
   }
   
   TEST_F(VectorTest, GrowPushDirect) {
     alloc.Reset();
     Vector<int> v{alloc};
   
     v.PushBack(10);
     v.PushBack(20);
     v.PushBack(30);
   
     ASSERT_EQ(v.GetSize(), 3u);
     ASSERT_EQ(v.GetMaxSize(), 4u);
     ASSERT_EQ(v[0], 10);
     ASSERT_EQ(v[1], 20);
     ASSERT_EQ(v[2], 30);
   }
   
   TEST_F(VectorTest, NormalPop) {
     alloc.Reset();
     Vector<int> v{16, alloc};
   
     v.PushBack(10);
     v.PushBack(20);
     v.PushBack(30);
   
     ASSERT_EQ(v.GetSize(), 3u);
     v.PopBack();
     ASSERT_EQ(v.GetSize(), 2u);
   }
   
   TEST_F(VectorTest, NormalRemove) {
     alloc.Reset();
     Vector<int> v{16, alloc};
   
     v.PushBack(10);
     v.PushBack(20);
     v.PushBack(30);
     v.PushBack(40);
   
     ASSERT_EQ(v.GetSize(), 4u);
   
     v.Remove(20);
   
     ASSERT_EQ(v.GetSize(), 3u);
     ASSERT_EQ(v[0], 10);
     ASSERT_EQ(v[1], 30);
     ASSERT_EQ(v[2], 40);
   
     v.Remove(30);
   
     ASSERT_EQ(v.GetSize(), 2u);
     ASSERT_EQ(v[0], 10);
     ASSERT_EQ(v[1], 40);
   }
   
   TEST_F(VectorTest, NormalFind) {
     alloc.Reset();
     Vector<int> v{16, alloc};
   
     v.PushBack(10);
     v.PushBack(20);
     v.PushBack(30);
   
     ASSERT_EQ(v.FindFirst(10), 0);
     ASSERT_EQ(v.FindFirst(20), 1);
     ASSERT_EQ(v.FindFirst(30), 2);
     ASSERT_EQ(v.FindFirst(99), -1);
   }
   
   TEST_F(VectorTest, Empty) {
     alloc.Reset();
     Vector<int> v{16, alloc};
   
     ASSERT_EQ(v.IsEmpty(), true);
   
     v.PushBack(10);
   
     ASSERT_EQ(v.IsEmpty(), false);
   
     v.PushBack(20);
     v.PushBack(30);
   
     ASSERT_EQ(v.IsEmpty(), false);
   }
   
   TEST_F(VectorTest, IteratorLoop) {
     alloc.Reset();
     Vector<int> v{16, alloc};
   
     v.PushBack(40);
     v.PushBack(10);
     v.PushBack(30);
     v.PushBack(20);
   
     int c = 0;
   
     for (auto i = v.Begin(); i != v.End(); ++i) {
       ASSERT_EQ(v[c], *i);
       c++;
     }
   }
   
   TEST_F(VectorTest, IteratorCount) {
     alloc.Reset();
     Vector<int> v{16, alloc};
   
     v.PushBack(40);
     v.PushBack(10);
     v.PushBack(30);
     v.PushBack(20);
     v.PushBack(10);
   
     const int count = 10;
   
     auto countResult = std::count(v.Begin(), v.End(), count);
     ASSERT_EQ(countResult, 2);
   }
   
   TEST_F(VectorTest, IteratorSort) {
     alloc.Reset();
     Vector<int> v{16, alloc};
   
     v.PushBack(40);
     v.PushBack(10);
     v.PushBack(30);
     v.PushBack(20);
   
     std::sort(v.Begin(), v.End());
   
     ASSERT_EQ(v[0], 10);
     ASSERT_EQ(v[1], 20);
     ASSERT_EQ(v[2], 30);
     ASSERT_EQ(v[3], 40);
   }
   
   TEST_F(VectorTest, ComplexTypeInitializerCtor) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 4;
     const int expectedCopyCtor   = 4;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 0;
     const int expectedMoveAssign = 0;
   
     Vector<ComplexType> v1 {{{10}, {20}, {30}, {40}}, alloc};
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(10, v1[0].Data());
     ASSERT_EQ(20, v1[1].Data());
     ASSERT_EQ(30, v1[2].Data());
     ASSERT_EQ(40, v1[3].Data());
     ASSERT_EQ(4U, v1.GetSize());
     ASSERT_EQ(4U, v1.GetMaxSize());
   
     // Compare with Ground Truth
     ComplexType::ResetStats();
   
     std::vector<ComplexType> v2{{{10}, {20}, {30}, {40}}};
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   }
   
   TEST_F(VectorTest, PushBackComplexTypesCopy) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 0;
     const int expectedCopyCtor   = 4;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 0;
     const int expectedMoveAssign = 0;
   
     Vector<ComplexType> v{16, alloc};
   
     const auto d1 = ComplexType{10};
     const auto d2 = ComplexType{20};
     const auto d3 = ComplexType{30};
     const auto d4 = ComplexType{40};
   
     v.PushBack(d1);
     v.PushBack(d2);
     v.PushBack(d3);
     v.PushBack(d4);
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(10, v[0].Data());
     ASSERT_EQ(20, v[1].Data());
     ASSERT_EQ(30, v[2].Data());
     ASSERT_EQ(40, v[3].Data());
   
     // Compare with Ground Truth
     ComplexType::ResetStats();
   
     const auto o1 = ComplexType{10};
     const auto o2 = ComplexType{20};
     const auto o3 = ComplexType{30};
     const auto o4 = ComplexType{40};
   
     std::vector<ComplexType> vOriginal;
     vOriginal.reserve(16);
     vOriginal.push_back(o1);
     vOriginal.push_back(o2);
     vOriginal.push_back(o3);
     vOriginal.push_back(o4);
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   }
   
   TEST_F(VectorTest, PushBackComplexTypesMove) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 4;
     const int expectedCopyCtor   = 0;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 4;
     const int expectedMoveAssign = 0;
   
     Vector<ComplexType> v{16, alloc};
   
     v.PushBack(ComplexType{10});
     v.PushBack(ComplexType{20});
     v.PushBack(ComplexType{30});
     v.PushBack(ComplexType{40});
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(10, v[0].Data());
     ASSERT_EQ(20, v[1].Data());
     ASSERT_EQ(30, v[2].Data());
     ASSERT_EQ(40, v[3].Data());
   
     // Compare with Ground Truth
     ComplexType::ResetStats();
   
     std::vector<ComplexType> vOriginal;
     vOriginal.reserve(16);
   
     vOriginal.push_back(ComplexType{10});
     vOriginal.push_back(ComplexType{20});
     vOriginal.push_back(ComplexType{30});
     vOriginal.push_back(ComplexType{40});
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   }
   
   TEST_F(VectorTest, PushBackComplexTypesMoveWithGrow) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 6;
     const int expectedCopyCtor   = 0;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 6;
     const int expectedMoveAssign = 0;
   
     Vector<ComplexType> v{2, alloc};
   
     v.PushBack(ComplexType{10});
     v.PushBack(ComplexType{20});
     v.PushBack(ComplexType{30});
     v.PushBack(ComplexType{40});
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(10, v[0].Data());
     ASSERT_EQ(20, v[1].Data());
     ASSERT_EQ(30, v[2].Data());
     ASSERT_EQ(40, v[3].Data());
   
     // Note: This test doesn't compare with std::vector because the growth is highly implementation dependent.
     // GCC doubles the capacity but MSVC does a 1.5 * capacity
   }
   
   TEST_F(VectorTest, EmplaceBackComplexTypes) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 0;
     const int expectedCopyCtor   = 0;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 0;
     const int expectedMoveAssign = 0;
   
     Vector<ComplexType> v{16, alloc};
   
     v.EmplaceBack(10);
     v.EmplaceBack(20);
     v.EmplaceBack(30);
     v.EmplaceBack(40);
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(10, v[0].Data());
     ASSERT_EQ(20, v[1].Data());
     ASSERT_EQ(30, v[2].Data());
     ASSERT_EQ(40, v[3].Data());
   
     // Compare with Ground Truth
     ComplexType::ResetStats();
   
     std::vector<ComplexType> vOriginal;
     vOriginal.reserve(16);
   
     vOriginal.emplace_back(10);
     vOriginal.emplace_back(20);
     vOriginal.emplace_back(30);
     vOriginal.emplace_back(40);
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   }
   
   TEST_F(VectorTest, PopComplexTypes) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 1;
     const int expectedCopyCtor   = 0;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 0;
     const int expectedMoveAssign = 0;
   
     Vector<ComplexType> v{16, alloc};
   
     v.EmplaceBack(10);
     v.EmplaceBack(20);
     v.EmplaceBack(30);
     v.EmplaceBack(40);
   
     v.PopBack();
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(10, v[0].Data());
     ASSERT_EQ(20, v[1].Data());
     ASSERT_EQ(30, v[2].Data());
   
     // Compare with Ground Truth
     ComplexType::ResetStats();
   
     std::vector<ComplexType> vOriginal;
     vOriginal.reserve(16);
   
     vOriginal.emplace_back(10);
     vOriginal.emplace_back(20);
     vOriginal.emplace_back(30);
     vOriginal.emplace_back(40);
   
     vOriginal.pop_back();
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   }
   
   TEST_F(VectorTest, CopyCtorTrivialTypes) {
     alloc.Reset();
     Vector<int> v1{16, alloc};
   
     v1.PushBack(10);
     v1.PushBack(20);
     v1.PushBack(30);
     v1.PushBack(40);
   
     Vector<int> v2(v1);
   
     ASSERT_EQ(10, v2[0]);
     ASSERT_EQ(20, v2[1]);
     ASSERT_EQ(30, v2[2]);
     ASSERT_EQ(40, v2[3]);
   }
   
   TEST_F(VectorTest, CopyCtorComplexTypes) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 0;
     const int expectedCopyCtor   = 4;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 0;
     const int expectedMoveAssign = 0;
   
     Vector<ComplexType> v1{16, alloc};
   
     v1.EmplaceBack(10);
     v1.EmplaceBack(20);
     v1.EmplaceBack(30);
     v1.EmplaceBack(40);
   
     Vector<ComplexType> v2(v1);
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(10, v2[0].Data());
     ASSERT_EQ(20, v2[1].Data());
     ASSERT_EQ(30, v2[2].Data());
     ASSERT_EQ(40, v2[3].Data());
     ASSERT_EQ(4U, v2.GetSize());
     ASSERT_EQ(16U, v2.GetMaxSize());
   
     // Compare with Ground Truth
     ComplexType::ResetStats();
   
     std::vector<ComplexType> v1Original;
     v1Original.reserve(16);
   
     v1Original.emplace_back(10);
     v1Original.emplace_back(20);
     v1Original.emplace_back(30);
     v1Original.emplace_back(40);
   
     std::vector<ComplexType> v2Original(v1Original);
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   }
   
   TEST_F(VectorTest, MoveCtorTrivialTypes) {
     alloc.Reset();
   
     Vector<int> v2 = Vector<int>(GetTrivialVector(alloc));
   
     ASSERT_EQ(10, v2[0]);
     ASSERT_EQ(20, v2[1]);
     ASSERT_EQ(30, v2[2]);
     ASSERT_EQ(40, v2[3]);
   }
   
   TEST_F(VectorTest, MoveCtorComplexTypes) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 0;
     const int expectedCopyCtor   = 0;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 0;
     const int expectedMoveAssign = 0;
   
     Vector<ComplexType> v2(GetComplexVector(alloc));
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(10, v2[0].Data());
     ASSERT_EQ(20, v2[1].Data());
     ASSERT_EQ(30, v2[2].Data());
     ASSERT_EQ(40, v2[3].Data());
     ASSERT_EQ(4U, v2.GetSize());
     ASSERT_EQ(16U, v2.GetMaxSize());
   
     // Compare with Ground Truth
     ComplexType::ResetStats();
   
     std::vector<ComplexType> v2Original(GetComplexStdVector());
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   }
   
   TEST_F(VectorTest, CopyAssignTrivialTypes) {
     alloc.Reset();
     Vector<int> v1{16, alloc};
   
     v1.PushBack(10);
     v1.PushBack(20);
     v1.PushBack(30);
     v1.PushBack(40);
   
     Vector<int> v2 = v1;
   
     ASSERT_EQ(10, v2[0]);
     ASSERT_EQ(20, v2[1]);
     ASSERT_EQ(30, v2[2]);
     ASSERT_EQ(40, v2[3]);
   }
   
   TEST_F(VectorTest, CopyAssignComplexTypes) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 0;
     const int expectedCopyCtor   = 4;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 0;
     const int expectedMoveAssign = 0;
   
     Vector<ComplexType> v1{16, alloc};
   
     v1.EmplaceBack(10);
     v1.EmplaceBack(20);
     v1.EmplaceBack(30);
     v1.EmplaceBack(40);
   
     Vector<ComplexType> v2 = v1;
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(10, v2[0].Data());
     ASSERT_EQ(20, v2[1].Data());
     ASSERT_EQ(30, v2[2].Data());
     ASSERT_EQ(40, v2[3].Data());
     ASSERT_EQ(4U, v2.GetSize());
     ASSERT_EQ(16U, v2.GetMaxSize());
   
     // Compare with Ground Truth
     ComplexType::ResetStats();
   
     std::vector<ComplexType> v1Original;
     v1Original.reserve(16);
   
     v1Original.emplace_back(10);
     v1Original.emplace_back(20);
     v1Original.emplace_back(30);
     v1Original.emplace_back(40);
   
     std::vector<ComplexType> v2Original = v1Original;
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   }
   
   TEST_F(VectorTest, MoveAssignTrivialTypes) {
     alloc.Reset();
   
     Vector<int> v2 = GetTrivialVector(alloc);
   
     ASSERT_EQ(10, v2[0]);
     ASSERT_EQ(20, v2[1]);
     ASSERT_EQ(30, v2[2]);
     ASSERT_EQ(40, v2[3]);
   }
   
   TEST_F(VectorTest, MoveAssignComplexTypes) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 0;
     const int expectedCopyCtor   = 0;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 0;
     const int expectedMoveAssign = 0;
   
     Vector<ComplexType> v2 = GetComplexVector(alloc);
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(10, v2[0].Data());
     ASSERT_EQ(20, v2[1].Data());
     ASSERT_EQ(30, v2[2].Data());
     ASSERT_EQ(40, v2[3].Data());
     ASSERT_EQ(4U, v2.GetSize());
     ASSERT_EQ(16U, v2.GetMaxSize());
   
     // Compare with Ground Truth
     ComplexType::ResetStats();
   
     std::vector<ComplexType> v2Original = GetComplexStdVector();
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   }
   
   TEST_F(VectorTest, ContructWithArguments) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 0;
     const int expectedCopyCtor   = 0;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 0;
     const int expectedMoveAssign = 0;
   
     Vector<ComplexType> v{ContainerExtent{4}, alloc, 5};
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(5, v[0].Data());
     ASSERT_EQ(5, v[1].Data());
     ASSERT_EQ(5, v[2].Data());
     ASSERT_EQ(5, v[3].Data());
   }
   
   TEST_F(VectorTest, VectorOfVector) {
     alloc.Reset();
     ComplexType::ResetStats();
   
     const int expectedCtors      = 4;
     const int expectedDtors      = 0;
     const int expectedCopyCtor   = 0;
     const int expectedCopyAssign = 0;
     const int expectedMoveCtor   = 0;
     const int expectedMoveAssign = 0;
   
     Vector<Vector<ComplexType>> v{ContainerExtent{2}, alloc, ContainerExtent{2}, alloc, 5};
   
     ASSERT_EQ(ComplexType::s_ctorCalls, expectedCtors);
     ASSERT_EQ(ComplexType::s_dtorCalls, expectedDtors);
   
     ASSERT_EQ(ComplexType::s_copyCtorCalls, expectedCopyCtor);
     ASSERT_EQ(ComplexType::s_copyAssignCalls, expectedCopyAssign);
   
     ASSERT_EQ(ComplexType::s_moveCtorCalls, expectedMoveCtor);
     ASSERT_EQ(ComplexType::s_moveAssignCalls, expectedMoveAssign);
   
     ASSERT_EQ(5, v[0][0].Data());
     ASSERT_EQ(5, v[0][1].Data());
     ASSERT_EQ(5, v[1][0].Data());
     ASSERT_EQ(5, v[1][1].Data());
   }
