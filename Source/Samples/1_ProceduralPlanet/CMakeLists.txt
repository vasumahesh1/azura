project(ProceduralPlanet)

# -----------  Main Target  -----------

set(
  SOURCES
    "Inc/ProceduralPlanet/AppRenderer.h"
    "Src/AppRenderer.cpp"
    "Src/main.cpp"
)

set(
  PROJECT_SHADERS
    "${PROJECT_SOURCE_DIR}/Shaders/Terrain.ps.slang|PS"
    "${PROJECT_SOURCE_DIR}/Shaders/Sky.ps.slang|PS"
    "${PROJECT_SOURCE_DIR}/Shaders/Water.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Shaders/ScreenQuad.vs.slang|VS"

    "${PROJECT_SOURCE_DIR}/Shaders/Noise.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/Noise.ps.slang|PS"
)

add_executable(ProceduralPlanet ${SOURCES})

AzuraDeployFolder(ProceduralPlanet Textures)

AzuraAddLoggingSupport(ProceduralPlanet 50)

AzuraAddSlangShaders(ProceduralPlanet VULKAN ${PROJECT_SHADERS})
AzuraAddSlangShaders(ProceduralPlanet GLSL ${PROJECT_SHADERS})

target_include_directories(ProceduralPlanet PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(ProceduralPlanet Camera VulkanRenderSystem Log glfw Math)