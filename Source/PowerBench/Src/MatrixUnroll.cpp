#include "PowerBench/MatrixUnroll.h"
#include "PowerBench/Utils.h"

using namespace Utils;

namespace {

// void BuildStringFromMatrix(double* Matrix, int NumRows, int NumColumns, double* outBuffer) {
//   const int R = NumRows;
//   const int C = NumColumns;
//   if (R <= 0 || C <= 0) return; // check for bad inputs
//   const int M = R * C;          // total number of matrix elements
//
//   // determine the number of spirals in the matrix
//   // choose the lesser of the two matrix dimensions and divide by two, rounding up
//   const int minDim     = R <= C ? R : C;
//   const int numSpirals = minDim % 2 == 0 ? minDim / 2 : minDim / 2 + 1;
//
//   // indices defining the rows and columns of spiral bounding box
//   // this box shrinks along one dimension with each line/column scan
//   int l = 0;
//   int r = C - 1;
//   int t = 0;
//   int b = R - 1;
//
//   int m = 0; // index tracking number of input matrix elements added to outBuffer
//   int n = 0; // index for char* outBuffer
//
//
//   for (int i = 0; i < numSpirals; ++i) {
//     // top row of spiral
//     for (int j = l; j <= r; ++j) {
//
//       int num_digits = std::strlen(tempBuffer);
//       std::memcpy(&outBuffer[n], tempBuffer, sizeof(double) * num_digits);
//       n += num_digits;
//       std::memcpy(&outBuffer[n], comma, sizeof(double) * 2);
//       n += 2;
//       m++;
//     }
//     t++;
//
//     // right column of spiral
//     for (int j = t; j <= b && m < M; ++j) {
//       _itoa_s(Matrix[j * C + r], tempBuffer, 33, 10);
//       int num_digits = std::strlen(tempBuffer);
//       std::memcpy(&outBuffer[n], tempBuffer, sizeof(double) * num_digits);
//       n += num_digits;
//       std::memcpy(&outBuffer[n], comma, sizeof(double) * 2);
//       n += 2;
//       m++;
//     }
//     r--;
//
//     // bottom row of spiral
//     for (int j = r; j >= l && m < M; --j) {
//       _itoa_s(Matrix[b * C + j], tempBuffer, 33, 10);
//       int num_digits = std::strlen(tempBuffer);
//       std::memcpy(&outBuffer[n], tempBuffer, sizeof(double) * num_digits);
//       n += num_digits;
//       std::memcpy(&outBuffer[n], comma, sizeof(double) * 2);
//       n += 2;
//       m++;
//     }
//     b--;
//
//     // left column of spiral
//     for (int j = b; j >= t && m < M; --j) {
//       _itoa_s(Matrix[j * C + l], tempBuffer, 33, 10);
//       int num_digits = std::strlen(tempBuffer);
//       std::memcpy(&outBuffer[n], tempBuffer, sizeof(double) * num_digits);
//       n += num_digits;
//       std::memcpy(&outBuffer[n], comma, sizeof(double) * 2);
//       n += 2;
//       m++;
//     }
//     l++;
//   }
//   outBuffer[n - 2] = '\0';
// }

} // namespace

BENCHMARK_DEFINE_F(MatrixUnrollFixture, BM_UnrollZach)(benchmark::State& st) {
  for (auto _ : st) {
    st.PauseTiming();
    Utils::ClearCache();
    st.ResumeTiming();

    const auto size = st.range(0);

  }
}

BENCHMARK_REGISTER_F(MatrixUnrollFixture, BM_UnrollZach)->RangeMultiplier(2)->Range(256, 256 << 3);
