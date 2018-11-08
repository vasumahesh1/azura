#pragma once

#include <benchmark/benchmark.h>

#include "PowerBench/Utils.h"

class MatrixUnrollFixture : public benchmark::Fixture
{
public:
  void TearDown(const benchmark::State&) override
  {
    delete[] m_data1;
    delete[] m_data2;
    delete[] m_result;
  }

  void SetUp(const benchmark::State& st) override
  {
    const auto fixtureSize = st.range(0);

    m_data1 = new double[fixtureSize * fixtureSize];
    Utils::CreateRandomMatrix(fixtureSize, m_data1);

    m_result = new double[fixtureSize * fixtureSize];
    Utils::CreateEmpty(fixtureSize, m_result);
  }

  double* m_data1;
  double* m_data2;
  double* m_result;
};
