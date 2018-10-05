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


# GLFW
AzuraDisableWarningsAsErrors()
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
set(GLFW_VULKAN_STATIC ON CACHE BOOL "Use the Vulkan loader statically linked into application" FORCE)
add_subdirectory(glfw)
AzuraSilenceClangTidy(glfw)
AzuraSilenceEnumGuard(glfw)
AzuraEnableWarningsAsErrors()

# STB Include
set(STB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stb/ CACHE STRING "STB Files Directory" FORCE)