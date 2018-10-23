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
AzuraSilenceClangTidy(benchmark_main)
AzuraSilenceEnumGuard(benchmark_main)
AzuraEnableWarningsAsErrors()


# GLFW
AzuraDisableWarningsAsErrors()
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "Use the Vulkan loader statically linked into application" FORCE)
add_subdirectory(glfw)
AzuraSilenceClangTidy(glfw)
AzuraSilenceEnumGuard(glfw)
AzuraEnableWarningsAsErrors()

# STB Include
set(STB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stb/ CACHE STRING "STB Files Directory" FORCE)


# GLTF SDK
if ("${BUILD_PLATFORM}" STREQUAL "Windows")
  if ("${BUILD_ARCH}" STREQUAL "64")
    set(CMAKE_GENERATOR_PLATFORM "x64")
  elseif("${BUILD_ARCH}" STREQUAL "32")
    set(CMAKE_GENERATOR_PLATFORM "Win32")
  endif()
endif()

AzuraDisableWarningsAsErrors()
set(ENABLE_UNIT_TESTS OFF CACHE BOOL "Disable Tests for GLTF SDK" FORCE)
set(ENABLE_SAMPLES OFF CACHE BOOL "Disable Samples for GLTF SDK" FORCE)
add_subdirectory(glTF-SDK)
AzuraSilenceClangTidy(GLTFSDK)
AzuraSilenceEnumGuard(GLTFSDK)
AzuraEnableWarningsAsErrors()