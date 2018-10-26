project(TestZone)

# -----------  Main Target  -----------

set(
  SOURCES
    "Inc/App/App.h"
    "Inc/App/Scene.h"
    "Inc/App/Common.h"
    "Inc/App/LightTexture.h"

    "Src/App.cpp"
    "Src/LightTexture.cpp"

    # Forward Renderer
    "Inc/Forward/ForwardScene.h"
    "Src/ForwardScene.cpp"

    # Forward Renderer with Compute
    "Inc/Forward/ForwardComputeScene.h"
    "Src/ForwardComputeScene.cpp"

    # Forward+ Renderer with Compute
    "Inc/ForwardPlus/ForwardPlusComputeScene.h"
    "Src/ForwardPlusComputeScene.cpp"

    "Src/main.cpp"
)

set(
  PROJECT_SHADERS
    "${PROJECT_SOURCE_DIR}/Shaders/Forward.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/Forward.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Shaders/ForwardCompute.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/ForwardCompute.ps.slang|PS"
    "${PROJECT_SOURCE_DIR}/Shaders/ForwardCompute.cs.slang|CS"

    # Forward Plus Compute
    "${PROJECT_SOURCE_DIR}/Shaders/ForwardPlusComputePass1.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/ForwardPlusComputePass1.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Shaders/ForwardPlusComputePass2.cs.slang|CS"

    "${PROJECT_SOURCE_DIR}/Shaders/ScreenQuad.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/ForwardPlusComputePass3.ps.slang|PS"
)

add_executable(DeferredRenderer ${SOURCES})

AzuraDeployFolder(DeferredRenderer Meshes)

AzuraAddLoggingSupport(DeferredRenderer 50)

target_compile_definitions(DeferredRenderer PUBLIC FORCE_LOGGING=1)

# Extension issue
# AzuraAddSlangShaders(DeferredRenderer GLSL ${PROJECT_SHADERS})
# AzuraAddSlangShaders(DeferredRenderer VULKAN ${PROJECT_SHADERS})

AzuraAddSlangShaders(DeferredRenderer HLSL ${PROJECT_SHADERS})
AzuraAddSlangShaders(DeferredRenderer DXBC ${PROJECT_SHADERS})

target_include_directories(DeferredRenderer PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(DeferredRenderer D3D12RenderSystem Camera Log glfw Math AssetManager)