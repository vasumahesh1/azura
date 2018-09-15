#pragma once

#include "Memory/HeapMemoryBuffer.h"
#include "Memory/MonotonicAllocator.h"
#include "gtest/gtest.h"
#include "Memory/RangeAllocator.h"
#include "Log/Log.h"

using namespace Azura::Memory;

class RendererTest : public ::testing::Test {
protected:
  RendererTest()
    : log_TestRenderer(Azura::Log("TestRenderer"))
  {}

  virtual ~RendererTest() = default;

  Azura::Log log_TestRenderer;
};
