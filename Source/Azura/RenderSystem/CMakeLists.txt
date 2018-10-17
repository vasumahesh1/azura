project(RenderSystem)


set(
    GENERIC_SOURCES

    # Headers
    "Inc/Generic/Constants.h"
    "Inc/Generic/Drawable.h"
    "Inc/Generic/GenericTypes.h"
    "Inc/Generic/GLFWWindow.h"
    "Inc/Generic/PoolPrimitives.h"
    "Inc/Generic/Renderer.h"
    "Inc/Generic/RenderSystem.h"
    "Inc/Generic/Shader.h"
    "Inc/Generic/TextureManager.h"
    "Inc/Generic/Window.h"
    "Inc/Generic/Windows/Win32GLFWWindow.h"

    # Sources
    "Src/Generic/Drawable.cpp"
    "Src/Generic/GenericTypes.cpp"
    "Src/Generic/GLFWWindow.cpp"
    "Src/Generic/PoolPrimitives.cpp"
    "Src/Generic/Renderer.cpp"
    "Src/Generic/Shader.cpp"
    "Src/Generic/TextureManager.cpp"
    "Src/Generic/Window.cpp"
    "Src/Generic/Windows/Win32GLFWWindow.cpp"
)

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
include("./D3D12.RenderSystem.cmake")