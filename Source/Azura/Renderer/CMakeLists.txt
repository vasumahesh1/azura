project (Renderer)

source_group(Generic REGULAR_EXPRESSION Inc/Generic/.*)
source_group(Generic REGULAR_EXPRESSION Src/Generic/.*)
source_group(D3D12 REGULAR_EXPRESSION Inc/D3D12/.*)
source_group(D3D12 REGULAR_EXPRESSION Src/D3D12/.*)
source_group(Vulkan REGULAR_EXPRESSION Inc/Vulkan/.*)
source_group(Vulkan REGULAR_EXPRESSION Src/Vulkan/.*)

set(VULKAN_SOURCES
	"Src/Vulkan/VulkanRenderer.cpp"
	"Src/Generic/Renderer.cpp"
)

AzuraAddLibrary(VulkanRenderer ${VULKAN_SOURCES})

target_compile_definitions(VulkanRenderer
	PUBLIC
		API_VULKAN=1
)

target_include_directories(VulkanRenderer
	PRIVATE
		${VULKAN_1_1_77_0_INCLUDE_DIR}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/Inc
)

target_link_libraries(VulkanRenderer
	Common
	${VULKAN_1_1_77_0_LIB64_DIR}/vulkan-1
)