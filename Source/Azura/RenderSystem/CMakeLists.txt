project(RenderSystem)

# TODO(vasumahesh1): Add Platform Support here

set(
  VULKAN_SOURCES
  # Include
  "Inc/Generic/Constants.h"
  "Inc/Generic/GenericTypes.h"
  "Inc/Generic/RawStorageFormat.h"
  "Inc/Generic/Renderer.h"
  "Inc/Generic/RenderSystem.h"
  "Inc/Generic/Shader.h"
  "Inc/Generic/Window.h"

  "Inc/Vulkan/VkCore.h"
  "Inc/Vulkan/VkMacros.h"
  "Inc/Vulkan/VkRenderer.h"
  "Inc/Vulkan/VkScopedBuffer.h"
  "Inc/Vulkan/VkScopedPipeline.h"
  "Inc/Vulkan/VkShader.h"
  "Inc/Vulkan/VkTypeMapping.h"
  "Inc/Vulkan/VkTypes.h"
  "Inc/Vulkan/VkWindow.h"

  # Source
  "Src/Generic/RawStorageFormat.cpp"
  "Src/Generic/Renderer.cpp"
  "Src/Generic/Shader.cpp"
  "Src/Generic/Window.cpp"

  "Src/Vulkan/VkCore.cpp"
  "Src/Vulkan/VkRenderer.cpp"
  "Src/Vulkan/VkRenderSystem.cpp"
  "Src/Vulkan/VkScopedBuffer.cpp"
  "Src/Vulkan/VkScopedPipeline.cpp"
  "Src/Vulkan/VkShader.cpp"
  "Src/Vulkan/VkTypeMapping.cpp"
  "Src/Vulkan/VkTypes.cpp"
  "Src/Vulkan/VkWin32Window.cpp"
)

source_group(Generic\\Inc REGULAR_EXPRESSION "Inc/Generic/.*")
source_group(Generic\\Src REGULAR_EXPRESSION "Src/Generic/.*")
source_group(D3D12\\Inc REGULAR_EXPRESSION "Inc/D3D12/.*")
source_group(D3D12\\Src REGULAR_EXPRESSION "Src/D3D12/.*")
source_group(Vulkan\\Inc REGULAR_EXPRESSION "Inc/Vulkan/.*")
source_group(Vulkan\\Src REGULAR_EXPRESSION "Src/Vulkan/.*")

add_library(VulkanRenderSystem ${VULKAN_SOURCES})

target_compile_definitions(VulkanRenderSystem PUBLIC API_VULKAN=1)

target_include_directories(
  VulkanRenderSystem
  PRIVATE ${BOOST_ROOT}/ ${VULKAN_1_1_77_0_INCLUDE_DIR}
  PUBLIC ${PROJECT_SOURCE_DIR}/Inc
)

target_link_libraries(VulkanRenderSystem Common ${VULKAN_1_1_77_0_LIB64_DIR}/vulkan-1)
