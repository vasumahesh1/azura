#include "Vector_test.h"
#include <algorithm>

using namespace Azura;
using namespace Azura::Containers;

TEST_F(VectorTest, Constructs) {
  Vector<int> v{ 16, alloc };
  ASSERT_EQ(v.GetSize(), 0u);
}

TEST_F(VectorTest, NormalPush) {
  alloc.Reset();
  Vector<int> v{ 16, alloc };

  v.PushBack(10);
  v.PushBack(20);
  v.PushBack(30);

  ASSERT_EQ(v.GetSize(), 3u);
  ASSERT_EQ(v[0], 10);
  ASSERT_EQ(v[1], 20);
  ASSERT_EQ(v[2], 30);
}

TEST_F(VectorTest, NormalPop) {
  alloc.Reset();
  Vector<int> v{ 16, alloc };

  v.PushBack(10);
  v.PushBack(20);
  v.PushBack(30);

  ASSERT_EQ(v.GetSize(), 3u);
  ASSERT_EQ(v.Pop(), 30);
  ASSERT_EQ(v.GetSize(), 2u);
}

TEST_F(VectorTest, NormalRemove) {
  alloc.Reset();
  Vector<int> v{ 16, alloc };

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
  Vector<int> v{ 16, alloc };

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
  Vector<int> v{ 16, alloc };

  ASSERT_EQ(v.IsEmpty(), true);

  v.PushBack(10);

  ASSERT_EQ(v.IsEmpty(), false);

  v.PushBack(20);
  v.PushBack(30);

  ASSERT_EQ(v.IsEmpty(), false);
}

TEST_F(VectorTest, IteratorLoop) {
  alloc.Reset();
  Vector<int> v{ 16, alloc };


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
  Vector<int> v{ 16, alloc };

  v.PushBack(40);
  v.PushBack(10);
  v.PushBack(30);
  v.PushBack(20);
  v.PushBack(10);

  int count = 10;

  auto countResult = std::count(v.Begin(), v.End(), count);
  ASSERT_EQ(countResult, 2);
}

TEST_F(VectorTest, IteratorSort) {
  alloc.Reset();
  Vector<int> v{ 16, alloc };

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
