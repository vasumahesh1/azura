#pragma once

#include <cstdint>

#include <array>
#include <vector>

#include "Containers/Vector.h"

#ifdef API_VULKAN
#include "Vulkan/VulkanConfig.h"
#endif

namespace AZ {
    struct DeviceRequirements
    {
        bool mDiscreteGPU{ false };
    };

    class Renderer {
    public:
        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;
        Renderer& operator=(const Renderer& rhs) = delete;
        Renderer& operator=(Renderer&& rhs) = delete;

        Instance CreateInstance() = 0;
        PhysicalDevice SelectPhysicalDevice(Instance instance, const DeviceRequirements& requirements) = 0;
        void CreateLogicalDevice() = 0;
        void CreateSwapChain() = 0;
        void CreateRenderPass() = 0;
        void CreateDescriptorSetLayout() = 0;
        void CreateGraphicsPipelineLayout() = 0;
        void CreateShaderModule(const Vector<U8>& code) const = 0;

    private:
    };
}