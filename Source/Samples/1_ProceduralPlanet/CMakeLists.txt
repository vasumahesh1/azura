project(ProceduralPlanet)

# -----------  Main Target  -----------

set(
  SOURCES
    "Inc/ProceduralPlanet/AppRenderer.h"
    "Src/AppRenderer.cpp"
    "Src/main.cpp"
)

add_executable(ProceduralPlanet ${SOURCES})

AzuraSetGLFWForTarget(ProceduralPlanet)

target_include_directories(ProceduralPlanet PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(ProceduralPlanet VulkanRenderSystem GLFW_STATIC)