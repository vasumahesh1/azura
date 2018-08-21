#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>
#include <vector>

#include "Containers/Vector.h"
#include "Generic/Renderer.h"
#include "VkCore.h"
#include "VkScopedBuffer.h"

namespace Azura {
namespace Vulkan {

class VkDrawable final : public Drawable {
public:
  VkDrawable(U32 vertexCount,
             U32 indexCount,
             Memory::Allocator& allocator);

private:
  Containers::Vector<BufferInfo> m_offset;
};

class VkDrawablePool final : public DrawablePool {

public:
  VkDrawablePool(U32 byteSize,
                 VkDevice device,
                 VkBufferUsageFlags usage,
                 VkMemoryPropertyFlags memoryProperties,
                 const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties,
                 Memory::Allocator& allocator);

  U32 CreateDrawable(const DrawableCreateInfo& createInfo) override;
  Azura::Drawable& GetDrawable(U32 id) override;

private:
  VkScopedBuffer m_buffer;
  Containers::Vector<VkDrawable> m_drawablePools;
};

class VkRenderer : public Renderer {
public:
  VkRenderer(const ApplicationInfo& appInfo,
             const DeviceRequirements& deviceRequirements,
             Memory::Allocator& allocator);
  ~VkRenderer();

  VkRenderer(const VkRenderer& other) = delete;
  VkRenderer(VkRenderer&& other) noexcept = delete;
  VkRenderer& operator=(const VkRenderer& other) = delete;
  VkRenderer& operator=(VkRenderer&& other) noexcept = delete;

  U32 CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) override;
  DrawablePool& GetDrawablePool(U32 id) override;
};
} // namespace Vulkan
} // namespace Azura

