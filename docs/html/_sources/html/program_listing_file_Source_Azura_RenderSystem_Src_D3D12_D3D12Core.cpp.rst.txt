
.. _program_listing_file_Source_Azura_RenderSystem_Src_D3D12_D3D12Core.cpp:

Program Listing for File D3D12Core.cpp
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_D3D12_D3D12Core.cpp>` (``Source\Azura\RenderSystem\Src\D3D12\D3D12Core.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "D3D12/D3D12Core.h"
   
   #include "Log/Log.h"
   #include "D3D12/D3D12Macros.h"
   #include "D3D12/D3D12TypeMapping.h"
   #include "D3D12/D3D12ScopedBuffer.h"
   #include "D3D12/D3D12ScopedCommandBuffer.h"
   
   using namespace Microsoft::WRL; // NOLINT
   
   namespace Azura {
   namespace D3D12 {
   
   namespace {
     constexpr UINT NVIDIA_GPU = 4318;
   } // namespace
   
   ComPtr<IDXGISwapChain1> D3D12Core::CreateSwapChain(const ComPtr<IDXGIFactory4>& factory,
                                                      const ComPtr<ID3D12CommandQueue>& commandQueue,
                                                      const void* windowHandle,
                                                      const SwapChainRequirements& swapChainRequirements,
                                                      const Log& log_D3D12RenderSystem) {
   
     const auto swapChainFormat = ToDXGI_FORMAT(swapChainRequirements.m_format);
     VERIFY_OPT(log_D3D12RenderSystem, swapChainFormat, "Unknown Swap Chain Format");
   
     // Describe and create the swap chain.
     DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
     swapChainDesc.BufferCount           = swapChainRequirements.m_framesInFlight;
     swapChainDesc.Width                 = swapChainRequirements.m_extent.m_width;
     swapChainDesc.Height                = swapChainRequirements.m_extent.m_height;
     swapChainDesc.Format                = swapChainFormat.value();
     swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
     swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
     swapChainDesc.SampleDesc.Count      = 1;
   
     ComPtr<IDXGISwapChain1> swapChain;
   
     VERIFY_D3D_OP(log_D3D12RenderSystem, factory->CreateSwapChainForHwnd(
       commandQueue.Get(),
       *reinterpret_cast<const HWND*>(windowHandle), // NOLINT
       &swapChainDesc,
       nullptr,
       nullptr,
       &swapChain
     ), "Failed to create D3D12 Swapchain");
   
     return swapChain;
   }
   
   void D3D12Core::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter, const Log& log_D3D12RenderSystem) {
     ComPtr<IDXGIAdapter1> adapter;
     *ppAdapter = nullptr;
   
     UNUSED(log_D3D12RenderSystem);
   
     for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
       DXGI_ADAPTER_DESC1 desc;
       adapter->GetDesc1(&desc);
   
       LOG_INFO(log_D3D12RenderSystem, LOG_LEVEL, "Found GPU: %ls", desc.Description);
       LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "Device ID: %d", desc.DeviceId);
       LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "Vendor ID: %d", desc.VendorId);
       LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "System Memory: %zu", desc.DedicatedSystemMemory);
       LOG_DEBUG(log_D3D12RenderSystem, LOG_LEVEL, "Video Memory: %zu", desc.DedicatedVideoMemory);
   
       if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0u) {
         // Don't select the Basic Render Driver adapter.
         continue;
       }
   
       if (desc.VendorId != NVIDIA_GPU)
       {
         continue;
       }
   
       // Check to see if the adapter supports Direct3D 12, but don't create the
       // actual device yet.
       if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
         
         LOG_INFO(log_D3D12RenderSystem, LOG_LEVEL, "Selected GPU: %ls", desc.Description);
         
         break;
       }
     }
   
     *ppAdapter = adapter.Detach();
   }
   
   void D3D12Core::CopyBuffer(ID3D12GraphicsCommandList* commandList,
                              const D3D12ScopedBuffer& dest,
                              const D3D12ScopedBuffer& src,
                              UINT64 size) {
     return CopyBuffer(commandList, dest, 0, src, 0, size);
   }
   
   void D3D12Core::CopyBuffer(ID3D12GraphicsCommandList* commandList,
                              const D3D12ScopedBuffer& dest,
                              UINT64 dstOffset,
                              const D3D12ScopedBuffer& src,
                              UINT64 srcOffset,
                              UINT64 size) {
     commandList->CopyBufferRegion(dest.Real(), dstOffset, src.Real(), srcOffset, size);
   }
   
   } // namespace D3D12
   } // namespace Azura
