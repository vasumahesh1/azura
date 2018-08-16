#pragma once
#include "gtest/gtest.h"
#include "MemoryAllocators/MonotonicAllocator.h"
#include "Containers/Vector.h"

using namespace EngineTools;

class VectorTest : public ::testing::Test {
 protected:

  VectorTest() {
  }

  virtual ~VectorTest() {
  }

  MonotonicAllocator<16> alloc{4};
};
