#pragma once
#include <vulkan/vulkan_core.h>
#include "Types.h"

namespace Azura {
namespace Vulkan {

class VkScopedBuffer {

public:
  VkScopedBuffer(VkDevice device,
                 VkBufferUsageFlags usage,
                 U32 bufferSize,
                 VkMemoryPropertyFlags memoryProperties,
                 const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties);

  VkScopedBuffer();

  void Create(VkDevice device,
              VkBufferUsageFlags usage,
              U32 bufferSize,
              VkMemoryPropertyFlags memoryProperties,
              const VkPhysicalDeviceMemoryProperties& phyDeviceMemoryProperties);

  void* MapMemory(VkDeviceSize size, VkDeviceSize offset) const;
  void UnMapMemory() const;

  VkBuffer Real() const;
  VkDeviceMemory Memory() const;
  VkDescriptorBufferInfo GetDescriptorInfo(U32 offset, U32 range) const;


private:
  VkDevice m_device;
  VkBuffer m_buffer{};
  VkDeviceMemory m_memory{};
};

} // namespace Vulkan
} // namespace Azura
