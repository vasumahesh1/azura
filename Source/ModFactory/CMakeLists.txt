project(ModFactory)

# -----------  Main Target  -----------

set(
  SOURCES
  "Inc/ModFactory/ModManager.h"
  "Inc/ModFactory/Mod.h"
  "Src/ModManager.cpp"
  "Src/Mod.cpp"
)

add_library(ModFactory ${SOURCES})

AzuraAddLoggingSupport(ModFactory 50)

target_include_directories(ModFactory PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

target_link_libraries(ModFactory PUBLIC Common Core Log)