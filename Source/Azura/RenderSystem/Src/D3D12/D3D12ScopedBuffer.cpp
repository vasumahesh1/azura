#include "D3D12/D3D12ScopedBuffer.h"
#include "D3D12/d3dx12.h"
#include "D3D12/D3D12Macros.h"


namespace Azura {
namespace D3D12 {

namespace {
SizeType Align(SizeType uLocation, SizeType uAlign) {
  if ((0 == uAlign) || (uAlign & (uAlign - 1)) != 0) {
    throw std::runtime_error("non-pow2 alignment");
  }

  return ((uLocation + (uAlign - 1)) & ~(uAlign - 1));
}
} // namespace

void D3D12ScopedBuffer::Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                               const D3D12_HEAP_PROPERTIES& heapProperties,
                               U32 size,
                               D3D12_RESOURCE_STATES resourceStates,
                               const Log& log_D3D12RenderSystem) {
  m_size = size;

  const auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_size);

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Creating Buffer of Size: %d", size);

  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateCommittedResource(
    &heapProperties,
    D3D12_HEAP_FLAG_NONE,
    &bufferDesc,
    resourceStates,
    nullptr,
    IID_PPV_ARGS(&m_buffer)), "Failed to create Buffer");

  void* pData;
  CD3DX12_RANGE readRange(0, 0);
  m_buffer->Map(0, &readRange, &pData);
  p_dataCur = p_dataBegin = reinterpret_cast<U8*>(pData); // NOLINT
  p_dataEnd = p_dataBegin + m_size;                       // NOLINT
}

HRESULT D3D12ScopedBuffer::SubAllocateFromBuffer(U32 size, U32 alignment) {
  p_dataCur = reinterpret_cast<UINT8*>(Align(reinterpret_cast<SizeType>(p_dataCur), SizeType(alignment))); // NOLINT
  return (p_dataCur + size > p_dataEnd) ? E_INVALIDARG : S_OK;                                             // NOLINT
}

U32 D3D12ScopedBuffer::AppendData(const void* pData, U32 byteSize, U32 alignment, const Log& log_D3D12RenderSystem) {
  VERIFY_D3D_OP(log_D3D12RenderSystem, SubAllocateFromBuffer(byteSize, alignment),
    "Failed to Sub allocate inside buffer");

  const auto byteOffset = GetCurrentOffset();

  std::memcpy(p_dataCur, pData, byteSize);
  p_dataCur += byteSize; // NOLINT

  return byteOffset;
}

ID3D12Resource* D3D12ScopedBuffer::Real() const {
  return m_buffer.Get();
}

U32 D3D12ScopedBuffer::GetSize() const {
  return m_size;
}

U32 D3D12ScopedBuffer::GetCurrentOffset() const {
  return U32(p_dataCur - p_dataBegin);
}
} // namespace D3D12
} // namespace Azura
