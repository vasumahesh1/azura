# YAML CPP
AzuraDisableWarningsAsErrors()
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Disable Yaml CPP Tests" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Disable Yaml CPP Build Tools" FORCE)
add_subdirectory(yaml-cpp)
AzuraSilenceClangTidy(yaml-cpp)
AzuraSilenceEnumGuard(yaml-cpp)
AzuraEnableWarningsAsErrors()

# Mathfu
add_library(MathFu INTERFACE)
target_include_directories(MathFu INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mathfu/include ${CMAKE_CURRENT_SOURCE_DIR}/mathfu/dependencies/vectorial/include)

# Google Benchmark
AzuraDisableWarningsAsErrors()
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable Benchmark Testing" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable Benchmark GTest" FORCE)
if("${BUILD_ARCH}" STREQUAL "32")
  set(BENCHMARK_BUILD_32_BITS ON CACHE BOOL "Build 32-bit Benchmark" FORCE)
endif()

add_subdirectory(benchmark)
AzuraSilenceClangTidy(benchmark)
AzuraSilenceEnumGuard(benchmark)
AzuraEnableWarningsAsErrors()