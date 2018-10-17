#include "D3D12/D3D12ScopedCommandBuffer.h"
#include "D3D12/D3D12Macros.h"
#include "Log/Log.h"


namespace Azura {
namespace D3D12 {

D3D12ScopedCommandBuffer::D3D12ScopedCommandBuffer(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
  D3D12_COMMAND_LIST_TYPE commandListType,
  const Log& log_D3D12RenderSystem) : m_commandListType(commandListType) {

  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateCommandAllocator(m_commandListType, IID_PPV_ARGS(&m_commandAllocator)), "Failed to create command allocator");
}

void D3D12ScopedCommandBuffer::CreateGraphicsCommandList(const Microsoft::WRL::ComPtr<ID3D12Device>& device, ID3D12PipelineState* pipelineState,
  const Log& log_D3D12RenderSystem) {
  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateCommandList(0, m_commandListType, m_commandAllocator.Get(), pipelineState, IID_PPV_ARGS(&m_commandList)), "Failed to create command allocator");
}

ID3D12GraphicsCommandList* D3D12ScopedCommandBuffer::GetGraphicsCommandList() const {
  return m_commandList.Get();
}

} // namespace D3D12
} // namespace Azura
