
.. _program_listing_file_Source_Azura_RenderSystem_Src_D3D12_D3D12RenderSystem.cpp:

Program Listing for File D3D12RenderSystem.cpp
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_D3D12_D3D12RenderSystem.cpp>` (``Source\Azura\RenderSystem\Src\D3D12\D3D12RenderSystem.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Generic/RenderSystem.h"
   #include "D3D12/D3D12Renderer.h"
   #include "D3D12/D3D12TextureManager.h"
   #include "Generic/Windows/Win32GLFWWindow.h"
   
   namespace Azura {
   
   std::unique_ptr<Renderer> RenderSystem::CreateRenderer(const ApplicationInfo& appInfo,
                                                          const DeviceRequirements& deviceRequirements,
                                                          const ApplicationRequirements& appRequirements,
                                                          const SwapChainRequirements& swapChainRequirement,
                                                          const RenderPassRequirements& renderPassRequirements,
                                                          const DescriptorRequirements& descriptorRequirements,
                                                          const ShaderRequirements& shaderRequirements,
                                                          Memory::Allocator& mainAllocator,
                                                          Memory::Allocator& drawAllocator,
                                                          Window& window) {
     return std::make_unique<D3D12::D3D12Renderer>(appInfo, deviceRequirements, appRequirements, swapChainRequirement,
                                                   renderPassRequirements, descriptorRequirements, shaderRequirements,
                                                   mainAllocator,
                                                   drawAllocator, window);
   }
   
   std::unique_ptr<TextureManager> RenderSystem::CreateTextureManager(const TextureRequirements& textureRequirements) {
     return std::make_unique<D3D12::D3D12TextureManager>(textureRequirements);
   }
   
   std::unique_ptr<Window> RenderSystem::CreateApplicationWindow(String title, U32 width, U32 height) {
     return std::make_unique<Win32GLFWWindow>(title, width, height);
   }
   
   } // namespace Azura
