project(Log)

# -----------  Main Target  -----------

set(
  SOURCES
  "Inc/Log/Log.h"
  "Src/Log.cpp"
)

add_library(Log ${SOURCES})

AzuraSilenceCRTSecureWarnings(Log)
AzuraSilenceClangTidy(Log)
AzuraSilenceEnumGuard(Log)

target_include_directories(Log PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

target_link_libraries(Log PUBLIC Common yaml-cpp Boost::log Boost::filesystem)