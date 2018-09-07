project(Common)

source_group(Memory\\Inc REGULAR_EXPRESSION Inc/Memory/.*)
source_group(Memory\\Src REGULAR_EXPRESSION Src/Memory/.*)
source_group(Containers\\Inc REGULAR_EXPRESSION Inc/Containers/.*)
source_group(Containers\\Src REGULAR_EXPRESSION Src/Containers/.*)
source_group(Utils\\Inc REGULAR_EXPRESSION Inc/Utils/.*)
source_group(Utils\\Src REGULAR_EXPRESSION Src/Utils/.*)

# -----------  Main Target  -----------

set(
  SOURCES
  # Memory
  "Inc/Memory/Allocator.h"
  "Src/Memory/Allocator.cpp"
  "Inc/Memory/MemoryBuffer.h"
  "Inc/Memory/MemoryFactory.h"
  "Src/Memory/MemoryBuffer.cpp"
  "Inc/Memory/HeapMemoryBuffer.h"
  "Src/Memory/HeapMemoryBuffer.cpp"
  "Inc/Memory/StackMemoryBuffer.h"
  "Inc/Memory/MonotonicAllocator.h"
  "Src/Memory/MonotonicAllocator.cpp"
  "Inc/Memory/RangeAllocator.h"
  "Src/Memory/RangeAllocator.cpp"

  # Containers
  "Inc/Containers/Array.h"
  "Inc/Containers/Vector.h"
  "Inc/Containers/ArrayBatch.h"

  # Compiler Warnings Includes
  "Inc/CompileWarnings/EnableSclSecureWarnings.h"
  "Inc/CompileWarnings/DisableSclSecureWarnings.h"
  "Inc/CompileWarnings/EnableSwitchEnumWarning.h"
  "Inc/CompileWarnings/DisableSwitchEnumWarning.h"

  # Utils
  "Inc/Types.h"
  "Inc/Utils/Platform.h"
  "Inc/Utils/Macros.h"
  "Inc/Utils/Exceptions.h"
  "Inc/Utils/FileReader.h"
  "Src/Utils/FileReader.cpp"
  "Inc/Utils/Hash.h"
  "Src/Utils/Hash.cpp"
)

add_library(Common ${SOURCES})

target_include_directories(Common PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

# -----------  Testing Target  -----------

add_executable(
  CommonTest
  "Test/test.cpp"
  "Test/MonotonicAllocator_test.h"
  "Test/MonotonicAllocator_test.cpp"
  "Test/RangeAllocator_test.h"
  "Test/RangeAllocator_test.cpp"
  "Test/Vector_test.h"
  "Test/Vector_test.cpp"
  "Test/Array_test.h"
  "Test/Array_test.cpp"
  "Test/Hash_test.h"
  "Test/Hash_test.cpp"
  "Test/HeapMemoryBuffer_test.h"
  "Test/HeapMemoryBuffer_test.cpp"
  "Test/StackMemoryBuffer_test.h"
  "Test/StackMemoryBuffer_test.cpp"
)

azurasilenceclangtidy(CommonTest)
azuraaddunittest(CommonTest)

target_link_libraries(CommonTest gtest_main Common)
