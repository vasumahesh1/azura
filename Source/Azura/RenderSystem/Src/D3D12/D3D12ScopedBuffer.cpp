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
}


void D3D12ScopedBuffer::Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
                               const D3D12_HEAP_PROPERTIES& heapProperties,
                               U32 size,
                               D3D12_RESOURCE_STATES resourceStates,
                               D3D12_RESOURCE_FLAGS resourceFlags,
                               const Log& log_D3D12RenderSystem) {
  m_size = size;

  const auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_size, resourceFlags);

  LOG_DBG(log_D3D12RenderSystem, LOG_LEVEL, "Creating Buffer of Size: %d", size);

  VERIFY_D3D_OP(log_D3D12RenderSystem, device->CreateCommittedResource(
    &heapProperties,
    D3D12_HEAP_FLAG_NONE,
    &bufferDesc,
    resourceStates,
    nullptr,
    IID_PPV_ARGS(&m_buffer)), "Failed to create Buffer");
}

void D3D12ScopedBuffer::Map() {
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

U32 D3D12ScopedBuffer::AppendTextureData(const void* pData,
                                         U32 byteSize,
                                         U32 alignment,
                                         U32 textureWidth,
                                         U32 textureRowPitch,
                                         const Log& log_D3D12RenderSystem) {
  const U32 numRows        = byteSize / textureWidth;
  const U32 scaledRowPitch = SCALE_SIZE(textureWidth, textureRowPitch);
  const U32 requiredSize   = scaledRowPitch * numRows;

  VERIFY_D3D_OP(log_D3D12RenderSystem, SubAllocateFromBuffer(requiredSize, alignment),
    "Failed to Sub allocate inside buffer");

  const auto byteOffset = GetCurrentOffset();

  const U8* textureData = reinterpret_cast<const U8*>(pData); // NOLINT

  for (U32 idx = 0; idx < numRows; ++idx) {
    std::memcpy(p_dataCur, textureData, textureWidth);

    p_dataCur += scaledRowPitch; // NOLINT
    textureData += textureWidth; // NOLINT
  }

  return byteOffset;
}

ID3D12Resource* D3D12ScopedBuffer::Real() const {
  return m_buffer.Get();
}

D3D12_SHADER_RESOURCE_VIEW_DESC D3D12ScopedBuffer::GetSRV() const {

  D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
  srvDesc.Format                          = DXGI_FORMAT_UNKNOWN;
  srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_BUFFER;
  srvDesc.Buffer.Flags                    = D3D12_BUFFER_SRV_FLAG_NONE; // NOLINT
  srvDesc.Buffer.FirstElement             = 0;                          // NOLINT
  srvDesc.Buffer.NumElements              = m_size / m_stride;          // NOLINT
  srvDesc.Buffer.StructureByteStride      = m_stride;                   // NOLINT

  return srvDesc;
}

D3D12_UNORDERED_ACCESS_VIEW_DESC D3D12ScopedBuffer::GetUAV() const {
  D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
  uavDesc.Format                           = DXGI_FORMAT_UNKNOWN;
  uavDesc.ViewDimension                    = D3D12_UAV_DIMENSION_BUFFER;
  uavDesc.Buffer.Flags                     = D3D12_BUFFER_UAV_FLAG_NONE; // NOLINT
  uavDesc.Buffer.FirstElement              = 0;                          // NOLINT
  uavDesc.Buffer.NumElements               = UINT(m_size / m_stride);    // NOLINT
  uavDesc.Buffer.StructureByteStride       = m_stride;                   // NOLINT

  return uavDesc;
}

U32 D3D12ScopedBuffer::GetStride() const {
  return m_stride;
}

void D3D12ScopedBuffer::SetStride(U32 value) {
  m_stride = value;
}

U32 D3D12ScopedBuffer::GetSize() const {
  return m_size;
}

U32 D3D12ScopedBuffer::GetCurrentOffset() const {
  return U32(p_dataCur - p_dataBegin);
}

void D3D12ScopedBuffer::Reset() {
  p_dataCur = p_dataBegin;
}

void D3D12ScopedBuffer::Transition(ID3D12GraphicsCommandList* commandList,
                                   D3D12_RESOURCE_STATES fromState,
                                   D3D12_RESOURCE_STATES toState) const {
  const auto resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_buffer.Get(), fromState, toState);
  commandList->ResourceBarrier(1, &resourceBarrier);
}
} // namespace D3D12
} // namespace Azura
