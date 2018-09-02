#include "StackMemoryBuffer_test.h"

using namespace Azura::Memory;

TEST(StackMemoryBufferTest, Constructs) {
  StackMemoryBuffer<32> target;

  ASSERT_NE(target.GetCurrentPtr(), 0u);
  ASSERT_NE(target.GetBasePtr(), 0u);
  ASSERT_EQ(target.GetSize(), 32u);
}

TEST(StackMemoryBufferTest, AllocateNormal) {
  Azura::U32 allocateAmount = 32;

  StackMemoryBuffer<128> target;

  auto p1 = target.GetCurrentPtr();

  auto p2 = target.Allocate(allocateAmount);

  auto p3 = target.GetCurrentPtr();

  ASSERT_NE(p2, nullptr);
  ASSERT_EQ(p3 - p1, allocateAmount);
}
