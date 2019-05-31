project(BaseApp)

# -----------  Main Target  -----------

set(
  SOURCES
  "Src/main.cpp"
)

add_executable(BaseApp ${SOURCES})

AzuraAddLoggingSupport(BaseApp 50)

target_include_directories(BaseApp PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

target_link_libraries(BaseApp PUBLIC BaseAppShared)