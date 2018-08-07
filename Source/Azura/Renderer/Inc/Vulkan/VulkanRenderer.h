#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <array>
#include <vector>

#include "Containers/Vector.h"
#include "Generic/Renderer.h"


using Instance = VkInstance;
using PhysicalDevice = VkPhysicalDevice;
using LogicalDevice = VkDevice;
using Queue = VkQueue;
using Swapchain = VkSwapchainKHR;

namespace AZ {
    class VulkanRenderer : public Renderer {
    public:
        VulkanRenderer(const VulkanRenderer& other) = delete;
        VulkanRenderer(VulkanRenderer&& other) = delete;
        VulkanRenderer& operator=(const VulkanRenderer& rhs) = delete;
        VulkanRenderer& operator=(VulkanRenderer&& rhs) = delete;

        Instance CreateInstance() override;
        PhysicalDevice SelectPhysicalDevice(Instance instance, const DeviceRequirements& requirements) override;
        void CreateLogicalDevice() override;
        void CreateSwapChain() override;
        void CreateRenderPass() override;
        void CreateDescriptorSetLayout() override;
        void CreateGraphicsPipelineLayout() override;
        void CreateShaderModule(const Vector<U8>& code) const override;

    private:
    };
}
