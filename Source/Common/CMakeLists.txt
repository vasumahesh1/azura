project (Common)

source_group(MemoryAllocators REGULAR_EXPRESSION Inc/MemoryAllocators/.*)
source_group(MemoryAllocators REGULAR_EXPRESSION Src/MemoryAllocators/.*)
source_group(Containers REGULAR_EXPRESSION Inc/Containers/.*)
source_group(Containers REGULAR_EXPRESSION Src/Containers/.*)
source_group(Utils REGULAR_EXPRESSION Inc/Utils/.*)
source_group(Utils REGULAR_EXPRESSION Src/Utils/.*)

set(SOURCES
    "Src/MemoryAllocators/Allocator.cpp"
    # "Src/MemoryAllocators/StackAllocator.cpp"
    "Src/MemoryAllocators/MemoryBuffer.cpp"
    "Src/MemoryAllocators/HeapMemoryBuffer.cpp"
    "Src/MemoryAllocators/MonotonicAllocator.cpp"
    # "Inc/MemoryAllocators/PoolAllocator.h"
    "Inc/Containers/Array.h"
    "Inc/Containers/Vector.h"
    "Inc/Types.h"
    "Inc/Utils/Macros.h"
    "Inc/Utils/Exceptions.h"
    "Inc/Utils/FileReader.h"
    "Src/Utils/FileReader.cpp"
	"Inc/Containers/ArrayBatch.h")

add_library(Common ${SOURCES})

target_include_directories(Common
    PUBLIC
        ${PROJECT_SOURCE_DIR}/Inc
)