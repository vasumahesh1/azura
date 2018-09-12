project(ProceduralPlanet)

# -----------  Main Target  -----------

set(
  SOURCES
    "Inc/ProceduralPlanet/AppRenderer.h"
    "Src/AppRenderer.cpp"
    "Src/main.cpp"
)

set(
  VULKAN_SHADERS
    "${PROJECT_SOURCE_DIR}/Shaders/Vulkan/test.vertex|-S vert"
    "${PROJECT_SOURCE_DIR}/Shaders/Vulkan/test.pixel|-S frag"
)

add_executable(ProceduralPlanet ${SOURCES})

AzuraSilenceWarningsForMathfu(ProceduralPlanet)
AzuraSetGLFWForTarget(ProceduralPlanet)
AzuraAddSPIRVShadersToTarget(ProceduralPlanet ${VULKAN_SHADERS})

target_include_directories(ProceduralPlanet PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(ProceduralPlanet VulkanRenderSystem Log GLFW_STATIC MathFu)