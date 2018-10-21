#include "D3D12/D3D12ScopedSampler.h"
#include "Log/Log.h"


namespace Azura {
namespace D3D12 {

void D3D12ScopedSampler::Create() {
  m_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
  m_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
  m_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
  m_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
  m_desc.MipLODBias = 0;
  m_desc.MaxAnisotropy = 0;
  m_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
  m_desc.MinLOD = 0.0f;
  m_desc.MaxLOD = D3D12_FLOAT32_MAX;
}

const D3D12_SAMPLER_DESC& D3D12ScopedSampler::GetDesc() const {
  return m_desc;
}
} // namespace D3D12
} // namespace Azura
