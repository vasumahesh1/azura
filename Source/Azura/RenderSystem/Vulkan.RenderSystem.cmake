# TODO(vasumahesh1):[PLATFORM]: Add Linux Support

set(VULKAN_SOURCES
    # Include
    "Inc/Vulkan/VkBasicDrawablePool.h"
    "Inc/Vulkan/VkCore.h"
    "Inc/Vulkan/VkDebug.h"
    "Inc/Vulkan/VkDrawablePool.h"
    "Inc/Vulkan/VkIndirectDrawablePool.h"
    "Inc/Vulkan/VkMacros.h"
    "Inc/Vulkan/VkPlatform.h"
    "Inc/Vulkan/VkRenderer.h"
    "Inc/Vulkan/VkScopedBuffer.h"
    "Inc/Vulkan/VkScopedImage.h"
    "Inc/Vulkan/VkScopedPipeline.h"
    "Inc/Vulkan/VkScopedRenderPass.h"
    "Inc/Vulkan/VkScopedSampler.h"
    "Inc/Vulkan/VkScopedSwapChain.h"
    "Inc/Vulkan/VkShader.h"
    "Inc/Vulkan/VkTextureManager.h"
    "Inc/Vulkan/VkTypeMapping.h"
    "Inc/Vulkan/VkTypes.h"

    # Source
    "Src/Vulkan/VkBasicDrawablePool.cpp"
    "Src/Vulkan/VkCore.cpp"
    "Src/Vulkan/VkDebug.cpp"
    "Src/Vulkan/VkDrawablePool.cpp"
    "Src/Vulkan/VkIndirectDrawablePool.cpp"
    "Src/Vulkan/VkRenderer.cpp"
    "Src/Vulkan/VkRenderSystem.cpp"
    "Src/Vulkan/VkScopedBuffer.cpp"
    "Src/Vulkan/VkScopedImage.cpp"
    "Src/Vulkan/VkScopedPipeline.cpp"
    "Src/Vulkan/VkScopedRenderPass.cpp"
    "Src/Vulkan/VkScopedSampler.cpp"
    "Src/Vulkan/VkScopedSwapChain.cpp"
    "Src/Vulkan/VkShader.cpp"
    "Src/Vulkan/VkTextureManager.cpp"
    "Src/Vulkan/VkTypeMapping.cpp"
    "Src/Vulkan/VkTypes.cpp"
    "Src/Vulkan/Windows/VkWin32Platform.cpp"
    "Src/Vulkan/Windows/VkWin32RenderSystem.cpp"
)

source_group(Generic\\Inc REGULAR_EXPRESSION "Inc/Generic/.*")
source_group(Generic\\Src REGULAR_EXPRESSION "Src/Generic/.*")
source_group(Vulkan\\Inc REGULAR_EXPRESSION "Inc/Vulkan/.*")
source_group(Vulkan\\Src REGULAR_EXPRESSION "Src/Vulkan/.*")

add_library(VulkanRenderSystem ${GENERIC_SOURCES} ${VULKAN_SOURCES})

AzuraAddLoggingSupport(VulkanRenderSystem 50)

target_compile_definitions(VulkanRenderSystem PUBLIC API_VULKAN=1)

target_include_directories(VulkanRenderSystem
                           PUBLIC ${PROJECT_SOURCE_DIR}/Inc ${STB_INCLUDE_DIR})

target_link_libraries(VulkanRenderSystem PUBLIC Common Containers Platform Log Core VULKAN_STATIC glfw)


# -----------  Testing Target  -----------

if (INCLUDE_TESTS OR PROJECT_BUILD)
  add_executable(
    VulkanRenderSystemTest
    "Test/test.cpp"
    "Test/RenderTestCases.h"
    "Test/RenderTestCases.cpp"
    "Test/Renderer_test.h"
    "Test/Renderer_test.cpp"
  )

  AzuraSilenceClangTidy(VulkanRenderSystemTest)
  AzuraAddGraphicsTest(VulkanRenderSystemTest)
  AzuraSilenceWarningsForMathfu(VulkanRenderSystemTest)

  AzuraAddSlangShaders(VulkanRenderSystemTest VULKAN ${RENDER_SYSTEM_TEST_SHADERS})
  AzuraAddSlangShaders(VulkanRenderSystemTest GLSL ${RENDER_SYSTEM_TEST_SHADERS})

  add_custom_target(VulkanRenderSystemTest_Textures COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Test/Textures ${CMAKE_CURRENT_BINARY_DIR}/Textures)
  add_dependencies(VulkanRenderSystemTest VulkanRenderSystemTest_Textures)

  add_custom_command(
        TARGET VulkanRenderSystemTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/ReferenceImages/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${PROJECT_SOURCE_DIR}/Test/ReferenceImages/
                ${PROJECT_BINARY_DIR}/ReferenceImages/)

  target_link_libraries(VulkanRenderSystemTest VulkanRenderSystem Log Math glfw gtest_main)
endif()