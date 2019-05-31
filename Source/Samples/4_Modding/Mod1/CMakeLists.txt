project(Mod1)

# -----------  Main Target  -----------

set(
  SOURCES
  "Inc/Mod1/Modification1.h"
  "Src/Modification1.cpp"
)

add_library(Mod1 SHARED ${SOURCES})

AzuraAddLoggingSupport(Mod1 50)

target_include_directories(Mod1 PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

target_link_libraries(Mod1 PUBLIC BaseAppShared)