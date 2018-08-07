cmake_minimum_required (VERSION 3.11 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# TODO: Add Platform Support
set(CLANG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/External/Windows/Tools/LLVM/x64/LLVM/ CACHE STRING "" FORCE)
set(CLANG_TIDY_EXECUTABLE ${CLANG_PATH}/bin/clang-tidy.exe CACHE STRING "" FORCE)

# Local Sets
# set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXECUTABLE}" "-checks=-*,clang-analyzer-*,-clang-analyzer-cplusplus*")
# set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXECUTABLE}" "-p ${CMAKE_BINARY_DIR}")

# VS Grouping
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wno-long-long -pedantic")
endif()

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES RELEASE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()


function(AzuraAddLibrary libraryName sources)
	add_library(${libraryName} ${sources})
endfunction(AzuraAddLibrary)

add_subdirectory(Source)