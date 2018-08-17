#pragma once
#include "gtest/gtest.h"
#include "Memory/HeapMemoryBuffer.h"
#include "Memory/MonotonicAllocator.h"
#include "Containers/Array.h"

using namespace Azura::Containers;
using namespace Azura::Memory;

class ArrayTest : public ::testing::Test {
 protected:

  ArrayTest() : buffer(Azura::Memory::HeapMemoryBuffer(1024)), alloc(Azura::Memory::MonotonicAllocator(buffer, 1024)) {
  }

  virtual ~ArrayTest() {
  }

  Azura::Memory::HeapMemoryBuffer buffer;
  Azura::Memory::MonotonicAllocator alloc;
};
