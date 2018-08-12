project (RenderSystem)

set(VULKAN_SOURCES
	"Inc/Vulkan/VkCore.h"
	"Src/Vulkan/VkCore.cpp"
	"Inc/Vulkan/VkRenderer.h"
	"Src/Vulkan/VkRenderer.cpp"
	"Inc/Vulkan/VkTypes.h"
	"Inc/Vulkan/VkMacros.h"
	"Src/Vulkan/VkTypes.cpp"
	"Inc/Vulkan/VkScopedBuffer.h"
	"Src/Vulkan/VkScopedBuffer.cpp"
	"Inc/Vulkan/VkTypeMapping.h"
	"Inc/Vulkan/VkShader.h"
	"Src/Vulkan/VkShader.cpp"
	"Inc/Vulkan/VkScopedPipeline.h"
	"Src/Vulkan/VkScopedPipeline.cpp"
	"Inc/Generic/GenericTypes.h"
	"Inc/Generic/RawStorageFormat.h"
	"Inc/Generic/Shader.h"
	"Src/Generic/Shader.cpp"
	"Inc/Generic/Constants.h"
	"Inc/Generic/Renderer.h"
	"Src/Generic/Renderer.cpp"
)

source_group(Generic REGULAR_EXPRESSION "(Inc|Src)/Generic/.*")
source_group(D3D12 REGULAR_EXPRESSION "(Inc|Src)/D3D12/.*")
source_group(Vulkan/Inc REGULAR_EXPRESSION "Inc/Vulkan/.*")
source_group(Vulkan/Src REGULAR_EXPRESSION "Src/Vulkan/.*")

add_library(VulkanRenderSystem ${VULKAN_SOURCES})

target_compile_definitions(VulkanRenderSystem
	PUBLIC
		API_VULKAN=1
)

target_include_directories(VulkanRenderSystem
	PRIVATE
		${BOOST_ROOT}/
		${VULKAN_1_1_77_0_INCLUDE_DIR}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/Inc
)

target_link_libraries(VulkanRenderSystem
	Common
	${VULKAN_1_1_77_0_LIB64_DIR}/vulkan-1
)