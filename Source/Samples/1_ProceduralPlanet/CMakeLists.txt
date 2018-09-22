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
    "${PROJECT_SOURCE_DIR}/Shaders/Vulkan/Terrain.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/Vulkan/Terrain.ps.slang|PS"
)

add_executable(ProceduralPlanet ${SOURCES})

AzuraAddLoggingSupport(ProceduralPlanet 50)

AzuraAddSlangShaders(ProceduralPlanet VULKAN ${PROJECT_SHADERS})
AzuraAddSlangShaders(ProceduralPlanet GLSL ${PROJECT_SHADERS})

target_include_directories(ProceduralPlanet PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(ProceduralPlanet VulkanRenderSystem Log glfw Math)