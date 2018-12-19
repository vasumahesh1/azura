
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Generic_Renderer.h:

Program Listing for File Renderer.h
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Generic_Renderer.h>` (``Source\Azura\RenderSystem\Inc\Generic\Renderer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "Constants.h"
   #include "Containers/Vector.h"
   #include "Drawable.h"
   #include "GenericTypes.h"
   #include "Core/RawStorageFormat.h"
   #include "ComputePool.h"
   
   namespace Azura {
   class Window;
   
   struct DrawablePoolCreateInfo;
   
   constexpr U32 GLOBAL_INFLIGHT_FRAMES = 2;
   
   class Renderer {
   public:
     Renderer(ApplicationInfo appInfo,
              const DeviceRequirements& deviceRequirements,
              ApplicationRequirements appRequirements,
              const SwapChainRequirements& swapChainRequirements,
              const DescriptorRequirements& descriptorRequirements,
              Memory::Allocator& mainAllocator,
              Memory::Allocator& drawAllocator,
              Window& window);
     virtual ~Renderer() = default;
   
     Renderer(const Renderer& other) = delete;
     Renderer(Renderer&& other) = delete;
     Renderer& operator=(const Renderer& rhs) = delete;
     Renderer& operator=(Renderer&& rhs) = delete;
   
     virtual String GetRenderingAPI() const = 0;
   
     virtual DrawablePool& CreateDrawablePool(const DrawablePoolCreateInfo& createInfo) = 0;
     virtual ComputePool& CreateComputePool(const ComputePoolCreateInfo& createInfo) = 0;
   
     virtual void Submit() = 0;
     virtual void RenderFrame() = 0;
   
     virtual void EnterRenderFrame();
     virtual void ExitRenderFrame();
   
     virtual void SnapshotFrame(const String& exportPath) const = 0;
   
     virtual void BindRenderTarget(U32 renderTargetId, const TextureDesc& desc, const U8* buffer) = 0;
     virtual void BindBufferTarget(U32 bufferTargetId, const U8* buffer) = 0;
   
   protected:
     const ApplicationInfo& GetApplicationInfo() const;
     const DeviceRequirements& GetDeviceRequirements() const;
     const ApplicationRequirements& GetApplicationRequirements() const;
     const SwapChainRequirements& GetSwapchainRequirements() const;
   
     U32 GetCurrentFrame() const;
     void SetCurrentFrame(U32 frameIdx);
   
     Containers::Vector<DescriptorSlot> m_descriptorSlots;
     Containers::Vector<DescriptorTableEntry> m_descriptorSetTable;
   
     DescriptorCount m_descriptorCount;
   
     std::reference_wrapper<Memory::Allocator> m_mainAllocator;
     std::reference_wrapper<Memory::Allocator> m_drawPoolAllocator;
   
     std::reference_wrapper<Window> m_window;
   
   private:
     virtual void AddShader(const ShaderCreateInfo& info) = 0;
   
     const ApplicationInfo m_applicationInfo;
     const DeviceRequirements m_deviceRequirements;
     const ApplicationRequirements m_appRequirements;
     SwapChainRequirements m_swapChainRequirements;
   
     U32 m_frameCount;
   };
   } // namespace Azura
