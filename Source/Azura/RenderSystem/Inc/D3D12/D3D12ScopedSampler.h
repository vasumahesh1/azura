#pragma once
#include "D3D12Core.h"


namespace Azura {
namespace D3D12 {

class D3D12ScopedSampler
{
public:
  void Create();
  const D3D12_SAMPLER_DESC& GetDesc() const;

private:
  D3D12_SAMPLER_DESC m_desc{};
};

} // namespace D3D12
} // namespace Azura