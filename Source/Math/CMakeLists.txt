project(Math)

# -----------  Main Target  -----------

set(
  SOURCES
  "Inc/Math/Core.h"
  "Inc/Math/Transform.h"
  "Src/Transform.cpp"
)

add_library(Math ${SOURCES})

AzuraSilenceWarningsForMathfu(Math)

target_include_directories(Math PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

target_link_libraries(Math PUBLIC Common MathFu)