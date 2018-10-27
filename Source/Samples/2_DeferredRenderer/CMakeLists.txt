project(DeferredRenderer)

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
    FORWARD_SHADERS
    "${PROJECT_SOURCE_DIR}/Shaders/Forward.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/Forward.ps.slang|PS"
)

set(
    FORWARD_COMPUTE_SHADERS
    "${PROJECT_SOURCE_DIR}/Shaders/ForwardCompute.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/ForwardCompute.ps.slang|PS"
    "${PROJECT_SOURCE_DIR}/Shaders/ForwardCompute.cs.slang|CS"
)

set(
    FORWARD_PLUS_COMPUTE_SHADERS
    "${PROJECT_SOURCE_DIR}/Shaders/ForwardPlusComputePass1.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/ForwardPlusComputePass1.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Shaders/ForwardPlusComputePass2.cs.slang|CS"

    "${PROJECT_SOURCE_DIR}/Shaders/ScreenQuad.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/ForwardPlusComputePass3.ps.slang|PS"
)

# 
# Forward
# -------------------------------
add_executable(ForwardRenderer ${SOURCES})

AzuraDeployFolder(ForwardRenderer Meshes)

AzuraAddLoggingSupport(ForwardRenderer 50)

target_compile_definitions(ForwardRenderer PUBLIC FORCE_LOGGING=1 FORWARD_SCENE=1)

# Extension issue
# AzuraAddSlangShaders(ForwardRenderer GLSL ${FORWARD_SHADERS})
# AzuraAddSlangShaders(ForwardRenderer VULKAN ${FORWARD_SHADERS})

AzuraAddSlangShaders(ForwardRenderer HLSL ${FORWARD_SHADERS})
AzuraAddSlangShaders(ForwardRenderer DXBC ${FORWARD_SHADERS})

target_include_directories(ForwardRenderer PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(ForwardRenderer D3D12RenderSystem Camera Log glfw Math AssetManager)

# 
# Forward Compute
# -------------------------------
add_executable(ForwardComputeRenderer ${SOURCES})

AzuraAddLoggingSupport(ForwardComputeRenderer 50)

target_compile_definitions(ForwardComputeRenderer PUBLIC FORCE_LOGGING=1 FORWARD_COMPUTE_SCENE=1)

# Extension issue
# AzuraAddSlangShaders(ForwardComputeRenderer GLSL ${FORWARD_COMPUTE_SHADERS})
# AzuraAddSlangShaders(ForwardComputeRenderer VULKAN ${FORWARD_COMPUTE_SHADERS})

AzuraAddSlangShaders(ForwardComputeRenderer HLSL ${FORWARD_COMPUTE_SHADERS})
AzuraAddSlangShaders(ForwardComputeRenderer DXBC ${FORWARD_COMPUTE_SHADERS})

target_include_directories(ForwardComputeRenderer PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(ForwardComputeRenderer D3D12RenderSystem Camera Log glfw Math AssetManager)

# 
# Forward Plus Compute
# -------------------------------
add_executable(ForwardPlusComputeRenderer ${SOURCES})

AzuraAddLoggingSupport(ForwardPlusComputeRenderer 50)

target_compile_definitions(ForwardPlusComputeRenderer PUBLIC FORCE_LOGGING=1 FORWARD_PLUS_COMPUTE_SCENE=1)

# Extension issue
# AzuraAddSlangShaders(ForwardPlusComputeRenderer GLSL ${FORWARD_PLUS_COMPUTE_SHADERS})
# AzuraAddSlangShaders(ForwardPlusComputeRenderer VULKAN ${FORWARD_PLUS_COMPUTE_SHADERS})

AzuraAddSlangShaders(ForwardPlusComputeRenderer HLSL ${FORWARD_PLUS_COMPUTE_SHADERS})
AzuraAddSlangShaders(ForwardPlusComputeRenderer DXBC ${FORWARD_PLUS_COMPUTE_SHADERS})

target_include_directories(ForwardPlusComputeRenderer PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(ForwardPlusComputeRenderer D3D12RenderSystem Camera Log glfw Math AssetManager)