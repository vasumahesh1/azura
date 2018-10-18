#include "D3D12/D3D12Drawable.h"
#include "D3D12/D3D12TypeMapping.h"

namespace Azura {
namespace D3D12 {

D3D12Drawable::D3D12Drawable(const DrawableCreateInfo& info,
                             U32 numVertexSlots,
                             U32 numInstanceSlots,
                             U32 numUniformSlots,
                             Memory::Allocator& allocator)
  : Drawable(info, numVertexSlots, numInstanceSlots, numUniformSlots, allocator),
    m_vertexBufferView(numVertexSlots, allocator),
    m_instanceBufferView(numInstanceSlots, allocator),
    m_constantBufferView(numUniformSlots, allocator),
    m_indexBufferView() {
}

void D3D12Drawable::CreateResourceViews(const Microsoft::WRL::ComPtr<ID3D12Device>& device, ID3D12Resource* parentBuffer, const Containers::Vector<VertexSlot>& vertexSlots, CD3DX12_CPU_DESCRIPTOR_HANDLE drawableHeapHandle, UINT heapElementSize, const Log& log_D3D12RenderSystem) {
  const auto gpuAddress = parentBuffer->GetGPUVirtualAddress();
  for (const auto& ubInfo : m_uniformBufferInfos) {
    D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {
      gpuAddress + ubInfo.m_offset, ubInfo.m_byteSize
    };

    device->CreateConstantBufferView(&constantBufferViewDesc, drawableHeapHandle);
    drawableHeapHandle.Offset(heapElementSize);
  }

  for (const auto& vbInfos : m_vertexBufferInfos) {
    D3D12_VERTEX_BUFFER_VIEW vertexBufferViewDesc = {
      gpuAddress + vbInfos.m_offset,
      vbInfos.m_byteSize,
      vertexSlots[vbInfos.m_binding].m_strideSize
    };

    m_vertexBufferView.PushBack(vertexBufferViewDesc);
  }

  for (const auto& ibInfos : m_instanceBufferInfos) {
    D3D12_VERTEX_BUFFER_VIEW vertexBufferViewDesc = {
      gpuAddress + ibInfos.m_offset,
      ibInfos.m_byteSize,
      vertexSlots[ibInfos.m_binding].m_strideSize
    };

    m_instanceBufferView.PushBack(vertexBufferViewDesc);
  }

  const auto format = ToDXGI_FORMAT(GetIndexType());
  VERIFY_OPT(log_D3D12RenderSystem, format, "D3D12Drawable: Unknown Format");

  m_indexBufferView = {
    gpuAddress + m_indexBufferInfo.m_offset,
    m_indexBufferInfo.m_byteSize,
    format.value()
  };
}

void D3D12Drawable::RecordCommands(ID3D12GraphicsCommandList* commandList, CD3DX12_GPU_DESCRIPTOR_HANDLE drawableHeapHandle, UINT heapElementSize, const Log& log_D3D12RenderSystem) {
  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "D3D12Drawable: Recording Commands for Drawable");
  
  commandList->IASetIndexBuffer(&m_indexBufferView);

  U32 idx = 0;
  for (const auto& vbInfos : m_vertexBufferInfos) {
    commandList->IASetVertexBuffers(vbInfos.m_binding, 1, &m_vertexBufferView[idx]);
    ++idx;
  }

  idx = 0;
  for (const auto& ibInfos : m_instanceBufferInfos) {
    commandList->IASetVertexBuffers(ibInfos.m_binding, 1, &m_instanceBufferView[idx]);
    ++idx;
  }

  idx = 0;
  for (const auto& ubInfo : m_uniformBufferInfos) {
    drawableHeapHandle.Offset(idx * heapElementSize);
    commandList->SetGraphicsRootDescriptorTable(ubInfo.m_set, drawableHeapHandle);
    ++idx;
  }

  commandList->DrawIndexedInstanced(GetIndexCount(), GetInstanceCount(), 0, 0, 0);
}
} // namespace D3D12
} // namespace Azura
