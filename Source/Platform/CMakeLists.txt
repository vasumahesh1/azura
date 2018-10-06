project(Platform)

source_group(Inc REGULAR_EXPRESSION Inc/.*)
source_group(Src REGULAR_EXPRESSION Src/.*)

# -----------  Main Target  -----------

set(
  SOURCES
  # Compiler Warnings Includes
  "Inc/CompileWarnings/EnableSclSecureWarnings.h"
  "Inc/CompileWarnings/DisableSclSecureWarnings.h"
  "Inc/CompileWarnings/EnableSwitchEnumWarning.h"
  "Inc/CompileWarnings/DisableSwitchEnumWarning.h"

  # Utils
  "Inc/Utils/Platform.h"
  "Inc/Utils/FileReader.h"
  "Src/Utils/FileReader.cpp"
)

add_library(Platform ${SOURCES})

target_link_libraries(Platform PUBLIC Containers)

target_include_directories(Platform PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
