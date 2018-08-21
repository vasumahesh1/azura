#include "Vulkan/VkRenderer.h"
#include "Utils/Macros.h"

using namespace Azura::Containers; // NOLINT - Freedom to use using namespace in CPP files.

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
    m_drawables(allocator) {
}

U32 VkDrawablePool::CreateDrawable(const DrawableCreateInfo& createInfo) {
  // TODO(vasumahesh1): Implement
  UNUSED(createInfo);
  return 0u;
}

Drawable& VkDrawablePool::GetDrawable(U32 id) {
  return m_drawables[id];
}

VkRenderer::VkRenderer(const ApplicationInfo& appInfo,
                       const DeviceRequirements& deviceRequirements,
                       Memory::Allocator& allocator,
                       VkWindow& window)
  : Renderer(appInfo, deviceRequirements, allocator),
    m_window(window),
    // TODO(vasumahesh1): Need to decide
    m_drawablePools(allocator) {

  // TODO(vasumahesh1): Implement
  // Vector<String> extensions(4, allocator);
  // m_window.GetInstanceExtensions(extensions);

  // VkCore::CreateInstance(GetApplicationInfo(), extensions);
}

VkRenderer::~VkRenderer() = default;

U32 VkRenderer::CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) {
  // TODO(vasumahesh1): Implement
  UNUSED(createInfo);
  return 0u;
}

DrawablePool& VkRenderer::GetDrawablePool(U32 id) {
  return m_drawablePools[id];
}

} // namespace Vulkan
} // namespace Azura
