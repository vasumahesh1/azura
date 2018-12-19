
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Vulkan_VkTypes.h:

Program Listing for File VkTypes.h
==================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Vulkan_VkTypes.h>` (``Source\Azura\RenderSystem\Inc\Vulkan\VkTypes.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include <vulkan/vulkan_core.h>
   #include <array>
   #include <vector>
   #include "Containers/Vector.h"
   #include "Types.h"
   #include "VkScopedImage.h"
   
   namespace Azura {
   namespace Vulkan {
   struct VkQueueIndices {
     int m_graphicsFamily{-1};
     int m_presentFamily{-1};
   
     // Special Transfer Queue for copying buffer data from
     // GPU (Host Accessible) to a GPU Optimized location that
     // may or may not be accessible by the host.
     int m_transferFamily{-1};
   
     bool m_isTransferQueueRequired;
   
     bool IsComplete() const;
   
     std::array<U32, 2> GetIndicesArray() const;
     U32 GetActiveSize() const;
   };
   
   struct SwapChainDeviceSupport {
     VkSurfaceCapabilitiesKHR m_capabilities{};
     Containers::Vector<VkSurfaceFormatKHR> m_formats;
     Containers::Vector<VkPresentModeKHR> m_presentModes;
   
     explicit SwapChainDeviceSupport(Memory::Allocator& allocator);
     bool IsSupported() const;
   };
   }  // namespace Vulkan
   }  // namespace Azura
