set(D3D12_SOURCES
    # Include
    "Inc/D3D12/D3D12Core.h"
    "Inc/D3D12/D3D12DrawablePool.h"
    "Inc/D3D12/D3D12Macros.h"
    "Inc/D3D12/D3D12Renderer.h"
    "Inc/D3D12/D3D12ScopedBuffer.h"
    "Inc/D3D12/D3D12ScopedImage.h"
    "Inc/D3D12/D3D12ScopedPipeline.h"
    "Inc/D3D12/D3D12ScopedShader.h"
    "Inc/D3D12/D3D12TextureManager.h"
    "Inc/D3D12/D3D12TypeMapping.h"
    "Inc/D3D12/D3D12ScopedCommandBuffer.h"

    "Inc/D3D12/d3dx12.h"

    # Source
    "Src/D3D12/D3D12Core.cpp"
    "Src/D3D12/D3D12DrawablePool.cpp"
    "Src/D3D12/D3D12Renderer.cpp"
    "Src/D3D12/D3D12RenderSystem.cpp"
    "Src/D3D12/D3D12ScopedBuffer.cpp"
    "Src/D3D12/D3D12ScopedImage.cpp"
    "Src/D3D12/D3D12ScopedPipeline.cpp"
    "Src/D3D12/D3D12ScopedShader.cpp"
    "Src/D3D12/D3D12TextureManager.cpp"
    "Src/D3D12/D3D12TypeMapping.cpp"
    "Src/D3D12/D3D12ScopedCommandBuffer.cpp"
)

source_group(Generic\\Inc REGULAR_EXPRESSION "Inc/Generic/.*")
source_group(Generic\\Src REGULAR_EXPRESSION "Src/Generic/.*")
source_group(D3D12\\Inc REGULAR_EXPRESSION "Inc/D3D12/.*")
source_group(D3D12\\Src REGULAR_EXPRESSION "Src/D3D12/.*")

add_library(D3D12RenderSystem ${GENERIC_SOURCES} ${D3D12_SOURCES})

AzuraAddLoggingSupport(D3D12RenderSystem 50)

target_compile_definitions(D3D12RenderSystem PUBLIC API_D3D12=1)

target_include_directories(D3D12RenderSystem
                           PUBLIC ${PROJECT_SOURCE_DIR}/Inc ${STB_INCLUDE_DIR})

target_link_libraries(D3D12RenderSystem PUBLIC Common Containers Platform Log Core glfw)


# -----------  Testing Target  -----------

# if (INCLUDE_TESTS OR PROJECT_BUILD)
#   add_executable(
#     D3D12RenderSystemTest
#     "Test/test.cpp"
#     "Test/RenderTestCases.h"
#     "Test/RenderTestCases.cpp"
#     "Test/Renderer_test.h"
#     "Test/Renderer_test.cpp"
#   )

#   AzuraSilenceClangTidy(D3D12RenderSystemTest)
#   AzuraAddGraphicsTest(D3D12RenderSystemTest)
#   AzuraSilenceWarningsForMathfu(D3D12RenderSystemTest)

#   AzuraAddSlangShaders(D3D12RenderSystemTest VULKAN ${RENDER_SYSTEM_TEST_SHADERS})
#   AzuraAddSlangShaders(D3D12RenderSystemTest GLSL ${RENDER_SYSTEM_TEST_SHADERS})

#   add_custom_target(D3D12RenderSystemTest_Textures COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Test/Textures ${CMAKE_CURRENT_BINARY_DIR}/Textures)
#   add_dependencies(D3D12RenderSystemTest D3D12RenderSystemTest_Textures)

#   add_custom_command(
#         TARGET D3D12RenderSystemTest POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/ReferenceImages/"
#         COMMAND ${CMAKE_COMMAND} -E copy_directory
#                 ${PROJECT_SOURCE_DIR}/Test/ReferenceImages/
#                 ${PROJECT_BINARY_DIR}/ReferenceImages/)

#   target_link_libraries(D3D12RenderSystemTest D3D12RenderSystem Log Math glfw gtest_main)
# endif()