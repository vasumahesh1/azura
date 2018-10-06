project(TestZone)

# -----------  Main Target  -----------

set(
  SOURCES
    "Inc/TestZone/AppRenderer.h"
    "Src/AppRenderer.cpp"
    "Src/main.cpp"
)

set(
  PROJECT_SHADERS
    "${PROJECT_SOURCE_DIR}/Shaders/Vulkan/TestZone.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/Vulkan/TestZone.ps.slang|PS"
)

add_executable(TestZone ${SOURCES})

AzuraAddLoggingSupport(TestZone 50)

AzuraAddSlangShaders(TestZone VULKAN ${PROJECT_SHADERS})
AzuraAddSlangShaders(TestZone GLSL ${PROJECT_SHADERS})

target_include_directories(TestZone PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(TestZone VulkanRenderSystem Log glfw Math)