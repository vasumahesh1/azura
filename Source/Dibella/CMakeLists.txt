# cmake_minimum_required (VERSION 3.2 FATAL_ERROR)

# include(CTest)

# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# add_subdirectory(src)
# add_subdirectory(test)

# project (Common)

# set(SOURCES
# 	"Src/MemoryAllocators/Allocator.cpp"
# 	# "Src/MemoryAllocators/StackAllocator.cpp"
# 	# "Src/MemoryAllocators/MemoryBuffer.cpp"
# 	# "Src/MemoryAllocators/HeapMemoryBuffer.cpp"
# 	"Inc/MemoryAllocators/MonotonicAllocator.h"
# 	# "Inc/MemoryAllocators/PoolAllocator.h"
# 	"Inc/Containers/Array.h"
# 	"Inc/Containers/Vector.h"
# 	"Inc/Types.h"
# )

# add_library(Common ${SOURCES})

# target_include_directories(Common
#     PUBLIC
#     	${PROJECT_SOURCE_DIR}/Inc
# )