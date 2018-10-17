#pragma once
#include "Generic/TextureManager.h"


namespace Azura {
namespace D3D12 {

class D3D12TextureManager : public TextureManager
{

public:
  explicit D3D12TextureManager(const TextureRequirements& requirements);
};

} // namespace D3D12
} // namespace Azura