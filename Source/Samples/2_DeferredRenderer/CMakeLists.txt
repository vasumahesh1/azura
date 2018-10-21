project(TestZone)

# -----------  Main Target  -----------

set(
  SOURCES
    "Inc/App/AppRenderer.h"
    "Inc/App/LightTexture.h"
    "Inc/App/LightDefinitions.h"
    "Src/AppRenderer.cpp"
    "Src/LightTexture.cpp"
    "Src/main.cpp"
)

set(
  PROJECT_SHADERS
    "${PROJECT_SOURCE_DIR}/Shaders/Forward.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/Forward.ps.slang|PS"
)

add_executable(DeferredRenderer ${SOURCES})

AzuraDeployFolder(DeferredRenderer Meshes)

AzuraAddLoggingSupport(DeferredRenderer 50)

AzuraAddSlangShaders(DeferredRenderer GLSL ${PROJECT_SHADERS})
AzuraAddSlangShaders(DeferredRenderer VULKAN ${PROJECT_SHADERS})

AzuraAddSlangShaders(DeferredRenderer HLSL ${PROJECT_SHADERS})
AzuraAddSlangShaders(DeferredRenderer DXBC ${PROJECT_SHADERS})

target_include_directories(DeferredRenderer PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(DeferredRenderer D3D12RenderSystem Camera Log glfw Math AssetManager)