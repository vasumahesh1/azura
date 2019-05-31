project(BaseAppShared)

# -----------  Main Target  -----------

set(
  SOURCES
  "Inc/BaseAppShared/GameMod.h"
  "Src/GameMod.cpp"
)

add_library(BaseAppShared ${SOURCES})

AzuraAddLoggingSupport(BaseAppShared 50)

target_include_directories(BaseAppShared PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

target_link_libraries(BaseAppShared PUBLIC Common Core Log ModFactory)