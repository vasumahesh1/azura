#pragma once

#include <random>
#include <vector>

using TestMatrix = std::vector<std::vector<double>>;

namespace Utils {
constexpr uint32_t CACHE_LINE_BYTES = 64;
constexpr uint32_t ELEMENTS_PER_LINE = CACHE_LINE_BYTES / sizeof(double);

constexpr size_t BIGGER_THAN_CACHE = 8 * 1024 * 1024;
inline auto g_dummyVec = std::vector<double>(BIGGER_THAN_CACHE);

const double MATRIX_VAL_MIN = -5.0;
const double MATRIX_VAL_MAX = 5.0;

inline void ClearCache() {
  for(int i = 0; i < BIGGER_THAN_CACHE; i++)
  {
     g_dummyVec[i] = rand();
  }
}

inline void CreateRandomMatrix(int64_t size, double* result) {
  std::random_device rd;
  std::mt19937 mt(rd());

  const std::uniform_real_distribution<double> randomValue(MATRIX_VAL_MIN, MATRIX_VAL_MAX);

  for(int64_t i = 0; i < size; i++)
  {
    for (int64_t j = 0; j < size; j++) {
      result[i * size + j] = randomValue(mt);
    }
  }
}

inline void CreateEmpty(int64_t size, double* result) {
  for(int64_t i = 0; i < size; i++)
  {
    for (int64_t j = 0; j < size; j++) {
      result[i * size + j] = 0.0;
    }
  }
}

} // namespace Utils