project(Core)

set(
  SOURCES
  "Inc/Core/AssetLocation.h"
  "Inc/Core/RawStorageFormat.h"
  "Inc/Core/Events.h"
  "Src/RawStorageFormat.cpp"
)

add_library(Core ${SOURCES})

target_include_directories(Core PUBLIC ${PROJECT_SOURCE_DIR}/Inc ${BOOST_ROOT}/include)
target_link_libraries(Core PUBLIC Common)
