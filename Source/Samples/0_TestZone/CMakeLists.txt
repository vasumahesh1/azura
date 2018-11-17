project(TestZone)

# -----------  Main Target  -----------

set(
  SOURCES
    "Inc/TestZone/AppRenderer.h"
    "Inc/TestZone/ClothMesh.h"
    "Src/AppRenderer.cpp"
    "Src/ClothMesh.cpp"
    "Src/main.cpp"
)

set(
  PROJECT_SHADERS
    # "${PROJECT_SOURCE_DIR}/Shaders/TestZone.vs.slang|VS"
    # "${PROJECT_SOURCE_DIR}/Shaders/TestZone.ps.slang|PS"

    # "${PROJECT_SOURCE_DIR}/Shaders/TestZone.Deferred.vs.slang|VS"
    # "${PROJECT_SOURCE_DIR}/Shaders/TestZone.Deferred.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Shaders/BasicRenderTest.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/BasicRenderTest.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Shaders/BasicInstancingTest.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/BasicInstancingTest.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Shaders/BasicTextureTest.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/BasicTextureTest.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Shaders/BasicDeferredTest.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/BasicDeferredTest.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Shaders/BasicDeferredTest.Deferred.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/BasicDeferredTest.Deferred.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Shaders/BasicCompute.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Shaders/BasicCompute.ps.slang|PS"
    "${PROJECT_SOURCE_DIR}/Shaders/BasicCompute.cs.slang|CS"

    "${PROJECT_SOURCE_DIR}/Shaders/Cloth.ps.slang|PS"
    "${PROJECT_SOURCE_DIR}/Shaders/Cloth.vs.slang|VS"
)

add_executable(TestZone ${SOURCES})

AzuraDeployFolder(TestZone Textures)

AzuraAddLoggingSupport(TestZone 50)

AzuraAddSlangShaders(TestZone GLSL ${PROJECT_SHADERS})
AzuraAddSlangShaders(TestZone VULKAN ${PROJECT_SHADERS})

AzuraAddSlangShaders(TestZone HLSL ${PROJECT_SHADERS})
AzuraAddSlangShaders(TestZone DXBC ${PROJECT_SHADERS})

target_include_directories(TestZone PUBLIC ${PROJECT_SOURCE_DIR}/Inc)
target_link_libraries(TestZone D3D12RenderSystem Camera Log glfw Math)