project(RenderSystem)

set(
  RENDER_SYSTEM_TEST_SHADERS
    "${PROJECT_SOURCE_DIR}/Test/Shaders/BasicRenderTest.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Test/Shaders/BasicRenderTest.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Test/Shaders/BasicInstancingTest.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Test/Shaders/BasicInstancingTest.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Test/Shaders/BasicTextureTest.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Test/Shaders/BasicTextureTest.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Test/Shaders/BasicDeferredTest.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Test/Shaders/BasicDeferredTest.ps.slang|PS"

    "${PROJECT_SOURCE_DIR}/Test/Shaders/BasicDeferredTest.Deferred.vs.slang|VS"
    "${PROJECT_SOURCE_DIR}/Test/Shaders/BasicDeferredTest.Deferred.ps.slang|PS"
)

include("./Vulkan.RenderSystem.cmake")