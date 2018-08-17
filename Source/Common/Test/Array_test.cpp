#include "Array_test.h"
#include <algorithm>

using namespace Azura;
using namespace Azura::Containers;

TEST_F(ArrayTest, Constructs) {
  Array<int, 16> a{ alloc };
  ASSERT_EQ(a.GetSize(), 0u);
}

TEST_F(ArrayTest, NormalPush) {
  alloc.Reset();
  Array<int, 16> a{ alloc };

  a.PushBack(10);
  a.PushBack(20);
  a.PushBack(30);

  ASSERT_EQ(a.GetSize(), 3u);
  ASSERT_EQ(a[0], 10);
  ASSERT_EQ(a[1], 20);
  ASSERT_EQ(a[2], 30);
}

TEST_F(ArrayTest, NormalPop) {
  alloc.Reset();
  Array<int, 16> a{ alloc };

  a.PushBack(10);
  a.PushBack(20);
  a.PushBack(30);

  ASSERT_EQ(a.GetSize(), 3u);
  ASSERT_EQ(a.Pop(), 30);
  ASSERT_EQ(a.GetSize(), 2u);
}

TEST_F(ArrayTest, NormalRemove) {
  alloc.Reset();
  Array<int, 16> a{ alloc };

  a.PushBack(10);
  a.PushBack(20);
  a.PushBack(30);
  a.PushBack(40);

  ASSERT_EQ(a.GetSize(), 4u);

  a.Remove(20);

  ASSERT_EQ(a.GetSize(), 3u);
  ASSERT_EQ(a[0], 10);
  ASSERT_EQ(a[1], 30);
  ASSERT_EQ(a[2], 40);

  a.Remove(30);

  ASSERT_EQ(a.GetSize(), 2u);
  ASSERT_EQ(a[0], 10);
  ASSERT_EQ(a[1], 40);
}

TEST_F(ArrayTest, NormalFind) {
  alloc.Reset();
  Array<int, 16> a{ alloc };

  a.PushBack(10);
  a.PushBack(20);
  a.PushBack(30);

  ASSERT_EQ(a.FindFirst(10), 0);
  ASSERT_EQ(a.FindFirst(20), 1);
  ASSERT_EQ(a.FindFirst(30), 2);
  ASSERT_EQ(a.FindFirst(99), -1);
}

TEST_F(ArrayTest, Empty) {
  alloc.Reset();
  Array<int, 16> a{ alloc };

  ASSERT_EQ(a.IsEmpty(), true);

  a.PushBack(10);

  ASSERT_EQ(a.IsEmpty(), false);

  a.PushBack(20);
  a.PushBack(30);

  ASSERT_EQ(a.IsEmpty(), false);
}

TEST_F(ArrayTest, IteratorLoop) {
  alloc.Reset();
  Array<int, 16> a{ alloc };

  a.PushBack(40);
  a.PushBack(10);
  a.PushBack(30);
  a.PushBack(20);

  int c = 0;
  for (auto i = a.Begin(); i != a.End(); ++i) {
    ASSERT_EQ(a[c], *i);
    c++;
  }
}

TEST_F(ArrayTest, IteratorSort) {
  alloc.Reset();
  Array<int, 16> a{ alloc };

  a.PushBack(40);
  a.PushBack(10);
  a.PushBack(30);
  a.PushBack(20);

  std::sort(a.Begin(), a.End());

  ASSERT_EQ(a[0], 10);
  ASSERT_EQ(a[1], 20);
  ASSERT_EQ(a[2], 30);
  ASSERT_EQ(a[3], 40);
}

TEST_F(ArrayTest, IteratorCount) {
  alloc.Reset();
  Array<int, 16> a{ alloc };

  a.PushBack(40);
  a.PushBack(10);
  a.PushBack(30);
  a.PushBack(20);
  a.PushBack(10);

  int count = 10;

  auto countResult = std::count(a.Begin(), a.End(), count);
  ASSERT_EQ(countResult, 2);
}
