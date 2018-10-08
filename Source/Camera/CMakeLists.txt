project(Camera)

# -----------  Main Target  -----------

set(
  SOURCES
  "Inc/Camera/Camera.h"
  "Src/Camera.cpp"

  "Inc/Camera/PolarCamera.h"
  "Src/PolarCamera.cpp"
)

add_library(Camera ${SOURCES})

target_include_directories(Camera PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

target_link_libraries(Camera PUBLIC Common Core Math Log)