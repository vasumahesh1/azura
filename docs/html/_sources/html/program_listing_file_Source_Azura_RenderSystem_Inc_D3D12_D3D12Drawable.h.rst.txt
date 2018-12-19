
.. _program_listing_file_Source_Azura_RenderSystem_Inc_D3D12_D3D12Drawable.h:

Program Listing for File D3D12Drawable.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_D3D12_D3D12Drawable.h>` (``Source\Azura\RenderSystem\Inc\D3D12\D3D12Drawable.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Generic/Drawable.h"
   #include "D3D12/D3D12Core.h"
   #include "D3D12/d3dx12.h"
   #include "Log/Log.h"
   
   namespace Azura {
   namespace D3D12 {
   
   class D3D12Drawable : public Drawable {
   
   public:
     D3D12Drawable(const DrawableCreateInfo& info,
                   U32 numVertexSlots,
                   U32 numInstanceSlots,
                   U32 numUniformSlots,
                   Memory::Allocator& allocator);
   
     void CreateResourceViews(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                              ID3D12Resource* parentBuffer,
                              const Containers::Vector<VertexSlot>& vertexSlots,
                              const Containers::Vector<std::reference_wrapper<const D3D12ScopedBuffer>>& gpuBuffers,
                              CD3DX12_CPU_DESCRIPTOR_HANDLE drawableHeapHandle,
                              UINT heapElementSize,
                              const Log& log_D3D12RenderSystem);
   
     void RecordCommands(ID3D12GraphicsCommandList* commandList,
                         CD3DX12_GPU_DESCRIPTOR_HANDLE drawableHeapHandle,
                         UINT heapElementSize,
                         const Containers::Vector<DescriptorTableEntry>& descriptorRootSignatureTable,
                         const Log& log_D3D12RenderSystem);
   
   private:
   
     Containers::Vector<D3D12_VERTEX_BUFFER_VIEW> m_vertexBufferView;
     Containers::Vector<D3D12_VERTEX_BUFFER_VIEW> m_instanceBufferView;
     Containers::Vector<D3D12_CONSTANT_BUFFER_VIEW_DESC> m_constantBufferView;
   
     D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
   };
   
   } // namespace D3D12
   } // namespace Azura
