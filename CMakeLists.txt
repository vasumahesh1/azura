cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

project(Mundus)

# The term "Mundus" is Latin for "world" or "universe."
#
# Mundus is the plane or realm of existence that encompasses Nirn,
# its moons, and attendant Aedric planets. It is surrounded by Oblivion
# and Oblivion is surrounded by Aetherius.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# TODO: Add Platform Support
set(CLANG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/External/Windows/Tools/LLVM/x64/LLVM/
    CACHE STRING ""
    FORCE)
set(CLANG_TIDY_EXECUTABLE ${CLANG_PATH}/bin/clang-tidy.exe
    CACHE STRING ""
    FORCE)

set(DEPLOY_CONFIG_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/Config/deploy_config/
    CACHE STRING "Default Folder used for all targets"
    FORCE)

include("./Azura.Macros.cmake")
include("./Azura.Functions.cmake")

# Install GTest
if (INCLUDE_TESTS OR PROJECT_BUILD)
  include("./GTest.cmake")
  AzuraSilenceClangTidy(gtest)
  AzuraSilenceClangTidy(gtest_main)
endif()

enable_testing()

# VS Grouping
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX
           REPLACE "/W[0-4]"
                   "/W4"
                   CMAKE_CXX_FLAGS
                   "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /wd4061 /wd4127 /D_SCL_SECURE_NO_WARNINGS")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
       CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
       CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")

  set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Werror -Wall -Wno-long-long -pedantic -Wswitch -Wextra"
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

if(PROJECT_BUILD)
  message(STATUS "Disabling all Warnings for Project Build")
  AzuraDisableWarningsAsErrors()
  AzuraGlobalSilenceEnumGuard()
endif()

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  message(STATUS "Building Debug Build")
  add_definitions(-DBUILD_DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_definitions(-DBUILD_RELEASE)
  message(STATUS "Building Release Build")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

function(AzuraAddLibrary libraryName sources)
  add_library(${libraryName} ${sources})
endfunction(AzuraAddLibrary)

add_subdirectory(Source)
