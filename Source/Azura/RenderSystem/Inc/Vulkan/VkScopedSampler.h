#pragma once

#include <vulkan/vulkan_core.h>
#include "Log/Log.h"

namespace Azura {
namespace Vulkan {

class VkScopedSampler {
public:
  VkScopedSampler(VkDevice device, Log logger);

  VkSampler Real() const;

  void CleanUp() const;

private:
  VkDevice m_device;

  VkSampler m_sampler{};

  const Log log_VulkanRenderSystem;
};

} // namespace Vulkan
} // namespace Azura
