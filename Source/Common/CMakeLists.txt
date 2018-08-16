project (Common)

source_group(Memory\\Inc REGULAR_EXPRESSION Inc/Memory/.*)
source_group(Memory\\Src REGULAR_EXPRESSION Src/Memory/.*)
source_group(Containers\\Inc REGULAR_EXPRESSION Inc/Containers/.*)
source_group(Containers\\Src REGULAR_EXPRESSION Src/Containers/.*)
source_group(Utils\\Inc REGULAR_EXPRESSION Inc/Utils/.*)
source_group(Utils\\Src REGULAR_EXPRESSION Src/Utils/.*)

# -----------  Main Target  -----------

set(SOURCES
    "Inc/Memory/Allocator.h"
    "Src/Memory/Allocator.cpp"
    
    # "Src/Memory/StackAllocator.cpp"
    
    "Inc/Memory/MemoryBuffer.h"
    "Src/Memory/MemoryBuffer.cpp"

    "Inc/Memory/HeapMemoryBuffer.h"
    "Src/Memory/HeapMemoryBuffer.cpp"
    
    "Inc/Memory/MonotonicAllocator.h"
    "Src/Memory/MonotonicAllocator.cpp"

    # "Inc/Memory/PoolAllocator.h"
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


# -----------  Testing Target  -----------


add_executable(
  CommonTest
    "Test/test.cpp"
    "Test/MonotonicAllocator_test.h"
    "Test/MonotonicAllocator_test.cpp"
)

AzuraSilenceClangTidy(CommonTest)
AzuraAddUnitTest(CommonTest)

target_link_libraries(CommonTest
    gtest_main
    Common
)
