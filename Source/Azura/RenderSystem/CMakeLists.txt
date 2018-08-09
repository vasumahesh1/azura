project (RenderSystem)

set(VULKAN_SOURCES
	"Inc/Vulkan/VulkanCore.h"
	"Src/Vulkan/VulkanCore.cpp"
	"Inc/Vulkan/VulkanRenderer.h"
	"Src/Vulkan/VulkanRenderer.cpp"
	"Inc/Generic/Types.h"
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