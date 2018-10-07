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

add_custom_target(TestZone_Textures COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Textures ${CMAKE_CURRENT_BINARY_DIR}/Textures)

add_executable(TestZone ${SOURCES})

add_dependencies(TestZone TestZone_Textures)

AzuraAddLoggingSupport(TestZone 50)

AzuraAddSlangShaders(TestZone VULKAN ${PROJECT_SHADERS})
AzuraAddSlangShaders(TestZone GLSL ${PROJECT_SHADERS})

target_include_directories(TestZone PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(TestZone VulkanRenderSystem Log glfw Math)