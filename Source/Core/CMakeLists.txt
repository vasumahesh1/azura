project(Core)

set(
  SOURCES
  "Inc/Core/RawStorageFormat.h"
  "Src/RawStorageFormat.cpp"
)

add_library(Core ${SOURCES})

target_include_directories(Core PUBLIC ${PROJECT_SOURCE_DIR}/Inc ${BOOST_ROOT}/)
target_link_libraries(Core PUBLIC Common)
