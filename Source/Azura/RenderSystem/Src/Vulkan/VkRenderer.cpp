#include "Vulkan/VkRenderer.h"

namespace Azura {
namespace Vulkan {

VkDrawable::VkDrawable(U32 vertexCount, U32 indexCount, Memory::Allocator& allocator)
  : Drawable(vertexCount, indexCount, allocator),
    m_offset(allocator) {
}

VkDrawablePool::VkDrawablePool(U32 byteSize,
                               VkDevice device,
                               VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags memoryProperties,
                               const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                               Memory::Allocator& allocator)
  : DrawablePool(byteSize),
    m_buffer(device, usage, byteSize, memoryProperties, phyDeviceMemoryProperties),
    m_drawablePools(allocator) {
}

U32 VkDrawablePool::CreateDrawable(const DrawableCreateInfo& createInfo) {
  return 0u;
}

Drawable& VkDrawablePool::GetDrawable(U32 id) {
}

VkRenderer::VkRenderer(const ApplicationInfo& appInfo,
                       const DeviceRequirements& deviceRequirements,
                       Memory::Allocator& allocator)
  : Renderer(appInfo, deviceRequirements, allocator) {
}

VkRenderer::~VkRenderer() = default;

U32 VkRenderer::CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) {
  return 0u;
}

DrawablePool& VkRenderer::GetDrawablePool(U32 id) {
}

} // namespace Vulkan
} // namespace Azura
