#pragma once
#include "D3D12/D3D12Core.h"


namespace Azura {
namespace D3D12 {
class D3D12ScopedCommandBuffer {

public:
  D3D12ScopedCommandBuffer(const Microsoft::WRL::ComPtr<ID3D12Device>& device, D3D12_COMMAND_LIST_TYPE commandListType, const Log& log_D3D12RenderSystem);

  void CreateGraphicsCommandList(const Microsoft::WRL::ComPtr<ID3D12Device>& device, ID3D12PipelineState* pipelineState, const Log& log_D3D12RenderSystem);
  ID3D12GraphicsCommandList* GetGraphicsCommandList() const;

private:
  D3D12_COMMAND_LIST_TYPE m_commandListType;

  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
};
} // namespace D3D12
} // namespace Azura
