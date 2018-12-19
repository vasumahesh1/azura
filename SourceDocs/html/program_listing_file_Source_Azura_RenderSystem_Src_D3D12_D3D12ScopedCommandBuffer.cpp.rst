
.. _program_listing_file_Source_Azura_RenderSystem_Src_D3D12_D3D12ScopedCommandBuffer.cpp:

Program Listing for File D3D12ScopedCommandBuffer.cpp
=====================================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Src_D3D12_D3D12ScopedCommandBuffer.cpp>` (``Source\Azura\RenderSystem\Src\D3D12\D3D12ScopedCommandBuffer.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "D3D12/D3D12ScopedCommandBuffer.h"
   #include "D3D12/D3D12Macros.h"
   #include "Log/Log.h"
   
   
   namespace Azura {
   namespace D3D12 {
   
   D3D12ScopedCommandBuffer::D3D12ScopedCommandBuffer(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
     D3D12_COMMAND_LIST_TYPE commandListType,
     const Log& log_D3D12RenderSystem)
     : m_commandListType(commandListType),
       m_fenceEvent() {
   
     VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateCommandAllocator(m_commandListType, IID_PPV_ARGS(&
       m_commandAllocator)), "Failed to create command allocator");
   }
   
   void D3D12ScopedCommandBuffer::CreateGraphicsCommandList(const Microsoft::WRL::ComPtr<ID3D12Device>& device, ID3D12PipelineState* pipelineState,
     const Log& log_D3D12RenderSystem) {
     VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateCommandList(0, m_commandListType, m_commandAllocator.Get(), pipelineState, IID_PPV_ARGS(&m_commandList)), "Failed to create command allocator");
   }
   
   ID3D12GraphicsCommandList* D3D12ScopedCommandBuffer::GetGraphicsCommandList() const {
     return m_commandList.Get();
   }
   
   void D3D12ScopedCommandBuffer::Execute(const Microsoft::WRL::ComPtr<ID3D12Device>& device, ID3D12CommandQueue* commandQueue, const Log& log_D3D12RenderSystem) {
   
     VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)
     ), "Failed to create fence");
     m_fenceValue++;
   
     // Create an event handle to use for frame synchronization.
     m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
     if (m_fenceEvent == nullptr) {
       VERIFY_D3D_OP(log_D3D12RenderSystem, HRESULT_FROM_WIN32(GetLastError()), "Fence Event Null");
     }
   
     ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
     commandQueue->ExecuteCommandLists(1, ppCommandLists); // NOLINT
   }
   
   void D3D12ScopedCommandBuffer::WaitForComplete(ID3D12CommandQueue* commandQueue, const Log& log_D3D12RenderSystem) {
     const U32 fence = m_fenceValue;
     VERIFY_D3D_OP(log_D3D12RenderSystem, commandQueue->Signal(m_fence.Get(), m_fenceValue), "Fence wait failed");
     m_fenceValue++;
   
     // Wait until the previous frame is finished.
     VERIFY_D3D_OP(log_D3D12RenderSystem, m_fence->SetEventOnCompletion(fence, m_fenceEvent), "Failed to set event completion on Fence");
     WaitForSingleObject(m_fenceEvent, INFINITE);
   }
   
   } // namespace D3D12
   } // namespace Azura
