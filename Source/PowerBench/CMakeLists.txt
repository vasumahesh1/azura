project(PowerBench)

# -----------  Benchmark Target  -----------

add_executable(
  PowerBench
  "Inc/PowerBench/Utils.h"

  # MatrixMultiplication
  "Inc/PowerBench/MatrixMultiplication.h"
  "Src/MatrixMultiplication.cpp"

  # MatrixUnroll
  "Inc/PowerBench/MatrixUnroll.h"
  "Src/MatrixUnroll.cpp"
)

target_include_directories(PowerBench PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

azurasilenceclangtidy(PowerBench)
target_link_libraries(PowerBench benchmark_main Containers)
