#pragma once
#include "D3D12/D3D12Core.h"


namespace Azura {
namespace D3D12 {
class D3D12ScopedCommandBuffer {

public:
  D3D12ScopedCommandBuffer(const Microsoft::WRL::ComPtr<ID3D12Device>& device, D3D12_COMMAND_LIST_TYPE commandListType, const Log& log_D3D12RenderSystem);

  void CreateGraphicsCommandList(const Microsoft::WRL::ComPtr<ID3D12Device>& device, ID3D12PipelineState* pipelineState, const Log& log_D3D12RenderSystem);
  ID3D12GraphicsCommandList* GetGraphicsCommandList() const;

  void Execute(const Microsoft::WRL::ComPtr<ID3D12Device>& device, ID3D12CommandQueue* commandQueue, const Log& log_D3D12RenderSystem);
  void WaitForComplete(ID3D12CommandQueue* commandQueue, const Log& log_D3D12RenderSystem);

private:
  D3D12_COMMAND_LIST_TYPE m_commandListType;

  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

  HANDLE m_fenceEvent;
  U32 m_fenceValue{0};
  Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;

};
} // namespace D3D12
} // namespace Azura
