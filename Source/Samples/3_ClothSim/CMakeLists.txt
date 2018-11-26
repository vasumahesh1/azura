project(ClothSim)

# -----------  Main Target  -----------

set(
  SOURCES
    "Inc/ClothSim/AppRenderer.h"
    "Src/AppRenderer.cpp"
    "Src/main.cpp"
)

set(
  PROJECT_SHADERS
    
    "${PROJECT_SOURCE_DIR}/Shaders/SolvingPass_Cloth_ApplyConstraints.cs.slang|CS"
    "${PROJECT_SOURCE_DIR}/Shaders/SolvingPass_Cloth_ComputeProjectedPositions.cs.slang|CS"
    "${PROJECT_SOURCE_DIR}/Shaders/SolvingPass_Cloth_ApplyDelta.cs.slang|CS"
    "${PROJECT_SOURCE_DIR}/Shaders/SolvingPass_Cloth_ComputePositions.cs.slang|CS"

    "${PROJECT_SOURCE_DIR}/Shaders/NormalsPass_Normalize.cs.slang|CS"
    
    "${PROJECT_SOURCE_DIR}/Shaders/ShadingPass_Cloth.ps.slang|PS"
    "${PROJECT_SOURCE_DIR}/Shaders/ShadingPass_Cloth.vs.slang|VS"

    "${PROJECT_SOURCE_DIR}/Shaders/ShadingPass_Sphere.ps.slang|PS"
    "${PROJECT_SOURCE_DIR}/Shaders/ShadingPass_Sphere.vs.slang|VS"

    "${PROJECT_SOURCE_DIR}/Shaders/ShadingPass_Plane.ps.slang|PS"
    "${PROJECT_SOURCE_DIR}/Shaders/ShadingPass_Plane.vs.slang|VS"
)

add_executable(ClothSim ${SOURCES})

AzuraDeployFolder(ClothSim Textures)

AzuraAddLoggingSupport(ClothSim 50)

# AzuraAddSlangShaders(ClothSim GLSL ${PROJECT_SHADERS})
# AzuraAddSlangShaders(ClothSim VULKAN ${PROJECT_SHADERS})

AzuraAddSlangShaders(ClothSim HLSL ${PROJECT_SHADERS})
AzuraAddSlangShaders(ClothSim DXBC ${PROJECT_SHADERS})

target_include_directories(ClothSim PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(ClothSim D3D12RenderSystem Camera Log Physics glfw Math)

AzuraDeployTarget(ClothSim)
AzuraDeployTargetFolder(ClothSim ${PROJECT_BINARY_DIR}/Shaders Shaders)
AzuraDeployTargetFolder(ClothSim ${PROJECT_BINARY_DIR}/Textures Textures)
