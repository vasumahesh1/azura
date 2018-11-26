project(Math)

# -----------  Main Target  -----------

set(
  SOURCES
  "Inc/Math/Core.h"
  "Inc/Math/Geometry.h"
  "Inc/Math/Icosphere.h"
  "Inc/Math/Plane.h"
  "Inc/Math/Transform.h"
  "Inc/Math/TransformComponent.h"
  "Src/Geometry.cpp"
  "Src/Icosphere.cpp"
  "Src/Plane.cpp"
  "Src/Transform.cpp"
  "Src/TransformComponent.cpp"
)

add_library(Math ${SOURCES})

AzuraSilenceWarningsForMathfu(Math)

target_include_directories(Math PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

target_link_libraries(Math PUBLIC Common Core MathFu)