project(Containers)

source_group(Inc REGULAR_EXPRESSION Inc/Containers/.*)
source_group(Src REGULAR_EXPRESSION Src/.*)

# -----------  Main Target  -----------

add_library(Containers INTERFACE)

target_include_directories(Containers INTERFACE ${PROJECT_SOURCE_DIR}/Inc)

target_link_libraries(Containers INTERFACE Common)

# -----------  Testing Target  -----------
if (INCLUDE_TESTS)
  add_executable(
    ContainersTest
    "Test/test.cpp"
    "Test/Vector_test.h"
    "Test/Vector_test.cpp"
    "Test/Array_test.h"
    "Test/Array_test.cpp"
  )

  azurasilenceclangtidy(ContainersTest)
  azuraaddunittest(ContainersTest)

  target_link_libraries(ContainersTest gtest_main Containers)
endif()

# -----------  Benchmark Target  -----------

add_executable(
  ContainersBenchmark
  "Benchmark/Vector_benchmark.h"
  "Benchmark/Vector_benchmark.cpp"
  "Benchmark/benchmark.cpp"
)

azurasilenceclangtidy(ContainersBenchmark)
target_link_libraries(ContainersBenchmark benchmark Containers)


add_executable(
  StdBenchmark
  "Benchmark/StdVector_benchmark.h"
  "Benchmark/StdVector_benchmark.cpp"
  "Benchmark/benchmark.cpp"
)

azurasilenceclangtidy(StdBenchmark)
target_link_libraries(StdBenchmark benchmark Containers)
