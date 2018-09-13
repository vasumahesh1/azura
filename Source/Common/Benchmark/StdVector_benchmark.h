#include <benchmark/benchmark.h>
#include <vector>

class VectorBenchmarkFixture : public benchmark::Fixture
{
public:
  VectorBenchmarkFixture() {}

  void TearDown(const benchmark::State&) override
  {}
};