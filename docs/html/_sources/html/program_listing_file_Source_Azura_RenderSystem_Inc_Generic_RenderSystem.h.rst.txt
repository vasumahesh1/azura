
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Generic_RenderSystem.h:

Program Listing for File RenderSystem.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Generic_RenderSystem.h>` (``Source\Azura\RenderSystem\Inc\Generic\RenderSystem.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Memory/Allocator.h"
   #include "Renderer.h"
   #include "TextureManager.h"
   
   namespace Azura {
   class Log;
   class Window;
   }
   
   namespace Azura {
   namespace RenderSystem {
   
   // TODO(vasumahesh1):[PERF]: On Random Heap.
   
   std::unique_ptr<Renderer> CreateRenderer(const ApplicationInfo& appInfo,
                                            const DeviceRequirements& deviceRequirements,
                                            const ApplicationRequirements& appRequirements,
                                            const SwapChainRequirements& swapChainRequirement,
                                            const RenderPassRequirements& renderPassRequirements,
                                            const DescriptorRequirements& descriptorRequirements,
                                            const ShaderRequirements& shaderRequirements,
                                            Memory::Allocator& mainAllocator,
                                            Memory::Allocator& drawAllocator,
                                            Window& window);
   
   std::unique_ptr<Window> CreateApplicationWindow(String title, U32 width, U32 height);
   
   std::unique_ptr<TextureManager> CreateTextureManager(const TextureRequirements& textureRequirements);
   
   } // namespace RenderSystem
   } // namespace Azura
