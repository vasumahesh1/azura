AzuraDisableWarningsAsErrors()
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Disable Yaml CPP Tests" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Disable Yaml CPP Build Tools" FORCE)
add_subdirectory(yaml-cpp)
AzuraSilenceClangTidy(yaml-cpp)
AzuraSilenceEnumGuard(yaml-cpp)
AzuraEnableWarningsAsErrors()


add_library(MathFu INTERFACE)
target_include_directories(MathFu INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mathfu/include ${CMAKE_CURRENT_SOURCE_DIR}/mathfu/dependencies/vectorial/include)