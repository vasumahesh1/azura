#include "Vector_test.h"
#include <algorithm>

TEST_F(VectorTest, Constructs) {
  Vector<int> v{ 16, &alloc };
  ASSERT_EQ(v.GetSize(), 0);
}

TEST_F(VectorTest, NormalPush) {
  alloc.Reset();
  Vector<int> v{ 16, &alloc };

  v.Push(10);
  v.Push(20);
  v.Push(30);

  ASSERT_EQ(v.GetSize(), 3);
  ASSERT_EQ(v[0], 10);
  ASSERT_EQ(v[1], 20);
  ASSERT_EQ(v[2], 30);
}

TEST_F(VectorTest, NormalPop) {
  alloc.Reset();
  Vector<int> v{ 16, &alloc };

  v.Push(10);
  v.Push(20);
  v.Push(30);

  ASSERT_EQ(v.GetSize(), 3);
  ASSERT_EQ(v.Pop(), 30);
  ASSERT_EQ(v.GetSize(), 2);
}

TEST_F(VectorTest, NormalRemove) {
  alloc.Reset();
  Vector<int> v{ 16, &alloc };

  v.Push(10);
  v.Push(20);
  v.Push(30);
  v.Push(40);

  int* backPtr1 = v.GetBackPtr();

  ASSERT_EQ(v.GetSize(), 4);

  v.Remove(20);

  int* backPtr2 = v.GetBackPtr();

  ASSERT_EQ((uintptr_t)backPtr1 - (uintptr_t)backPtr2, sizeof(int));

  ASSERT_EQ(v.GetSize(), 3);
  ASSERT_EQ(v[0], 10);
  ASSERT_EQ(v[1], 30);
  ASSERT_EQ(v[2], 40);

  v.Remove(30);

  ASSERT_EQ(v.GetSize(), 2);
  ASSERT_EQ(v[0], 10);
  ASSERT_EQ(v[1], 40);
}

TEST_F(VectorTest, NormalFind) {
  alloc.Reset();
  Vector<int> v{ 16, &alloc };

  v.Push(10);
  v.Push(20);
  v.Push(30);

  ASSERT_EQ(v.Find(10), 0);
  ASSERT_EQ(v.Find(20), 1);
  ASSERT_EQ(v.Find(30), 2);
  ASSERT_EQ(v.Find(99), -1);
}

TEST_F(VectorTest, Empty) {
  alloc.Reset();
  Vector<int> v{ 16, &alloc };

  ASSERT_EQ(v.IsEmpty(), true);

  v.Push(10);

  ASSERT_EQ(v.IsEmpty(), false);

  v.Push(20);
  v.Push(30);

  ASSERT_EQ(v.IsEmpty(), false);
}

TEST_F(VectorTest, IteratorLoop) {
  alloc.Reset();
  Vector<int> v{ 16, &alloc };


  v.Push(40);
  v.Push(10);
  v.Push(30);
  v.Push(20);

  int c = 0;
  for (auto i = v.Begin(); i != v.End(); ++i) {
    ASSERT_EQ(v[c], *i);
    c++;
  }
}

TEST_F(VectorTest, IteratorSort) {
  alloc.Reset();
  Vector<int> v{ 16, &alloc };

  v.Push(40);
  v.Push(10);
  v.Push(30);
  v.Push(20);

  std::sort(v.Begin(), v.End());

  ASSERT_EQ(v[0], 10);
  ASSERT_EQ(v[1], 20);
  ASSERT_EQ(v[2], 30);
  ASSERT_EQ(v[3], 40);
}
