#pragma once

#include <benchmark/benchmark.h>

#include "Memory/MemoryFactory.h"
#include "Memory/MonotonicAllocator.h"

class VectorBenchmarkFixture : public benchmark::Fixture
{
public:
  VectorBenchmarkFixture()
    : buffer(Azura::Memory::HeapMemoryBuffer(0x8000000)),
      alloc(Azura::Memory::MonotonicAllocator(buffer, 0x8000000)) {}

  void TearDown(const benchmark::State&) override
  {
    alloc.Reset();
  }

public:
  Azura::Memory::HeapMemoryBuffer buffer;
  Azura::Memory::MonotonicAllocator alloc;
};