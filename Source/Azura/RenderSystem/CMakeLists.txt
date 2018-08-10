project (RenderSystem)

set(VULKAN_SOURCES
	"Inc/Vulkan/VkCore.h"
	"Src/Vulkan/VkCore.cpp"
	"Inc/Vulkan/VkRenderer.h"
	"Src/Vulkan/VkRenderer.cpp"
	"Inc/Vulkan/VkTypes.h"
	"Src/Vulkan/VkTypes.cpp"
	"Inc/Vulkan/VkTypeMapping.h"
	"Src/Vulkan/VkTypeMapping.cpp"
	"Inc/Generic/GenericTypes.h"
	"Inc/Generic/RawStorageFormat.h"
	"Src/Generic/RawStorageFormat.cpp"
	"Inc/Generic/Constants.h"
	"Inc/Generic/Renderer.h"
	"Src/Generic/Renderer.cpp"
)

source_group(Generic REGULAR_EXPRESSION Inc/Generic/.*)
source_group(Generic REGULAR_EXPRESSION Src/Generic/.*)
source_group(D3D12 REGULAR_EXPRESSION Inc/D3D12/.*)
source_group(D3D12 REGULAR_EXPRESSION Src/D3D12/.*)
source_group(Vulkan REGULAR_EXPRESSION Inc/Vulkan/.*)
source_group(Vulkan REGULAR_EXPRESSION Src/Vulkan/.*)

add_library(VulkanRenderSystem ${VULKAN_SOURCES})

target_compile_definitions(VulkanRenderSystem
	PUBLIC
		API_VULKAN=1
)

target_include_directories(VulkanRenderSystem
	PRIVATE
		${VULKAN_1_1_77_0_INCLUDE_DIR}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/Inc
)

target_link_libraries(VulkanRenderSystem
	Common
	${VULKAN_1_1_77_0_LIB64_DIR}/vulkan-1
)