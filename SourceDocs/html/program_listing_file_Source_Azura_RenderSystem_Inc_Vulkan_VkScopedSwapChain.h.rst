
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Vulkan_VkScopedSwapChain.h:

Program Listing for File VkScopedSwapChain.h
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Vulkan_VkScopedSwapChain.h>` (``Source\Azura\RenderSystem\Inc\Vulkan\VkScopedSwapChain.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <vulkan/vulkan_core.h>
   #include "Types.h"
   #include <Containers/Vector.h>
   #include "VkScopedImage.h"
   
   
   namespace Azura {
   struct SwapChainRequirements;
   }
   
   namespace Azura {
   namespace Vulkan {
   struct VkQueueIndices;
   struct SwapChainDeviceSupport;
   
   class VkScopedSwapChain {
   public:
     VkScopedSwapChain(Memory::Allocator& allocator, Log logger);
   
     void Create(VkDevice device,
                 VkPhysicalDevice physicalDevice,
                 VkQueue graphicsQueue,
                 VkCommandPool graphicsCommandPool,
                 VkSurfaceKHR surface,
                 const VkQueueIndices& queueIndices,
                 const SwapChainDeviceSupport& swapChainSupport,
                 const SwapChainRequirements& swapChainRequirement,
                 const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties);
   
     void CleanUp(VkDevice device);
     VkSwapchainKHR Real() const;
   
     VkFormat GetSurfaceFormat() const;
     VkFormat GetDepthFormat() const;
   
     bool HasDepthSupport() const;
   
     const VkExtent2D& GetExtent() const;
     const VkScopedImage& GetImage(int frameIdx) const;
     const VkScopedImage& GetDepthImage() const;
   
     const Containers::Vector<VkScopedImage>& GetAllImages() const;
   
   private:
     const Log log_VulkanRenderSystem;
   
     VkSwapchainKHR m_swapChain;
     VkExtent2D m_extent;
     VkSurfaceFormatKHR m_surfaceFormat;
     U32 m_imageCount{0};
     bool m_hasDepthSupport{false};
   
     Containers::Vector<VkScopedImage> m_images;
     VkScopedImage m_depthImage;
   };
   
   } // namespace Vulkan
   } // namespace Azura
