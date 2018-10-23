project(AssetManager)

# -----------  Main Target  -----------

set(
  SOURCES
  "Inc/AssetManager/Common.h"
  "Inc/AssetManager/GLTFLoader.h"
  "Src/GLTFLoader.cpp"
)

add_library(AssetManager ${SOURCES})

target_include_directories(AssetManager PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

target_link_libraries(AssetManager PUBLIC Common Core Log GLTFSDK Boost::filesystem)