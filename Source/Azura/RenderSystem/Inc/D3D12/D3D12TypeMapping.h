#pragma once

#include <optional>
#include "Generic/GenericTypes.h"
#include "Core/RawStorageFormat.h"
#include "D3D12Core.h"

namespace Azura {
namespace D3D12 {
namespace Impl
{
#define CREATE_MAPPER_H(FROM_FORMAT, TO_FORMAT) std::optional<TO_FORMAT> To##TO_FORMAT(FROM_FORMAT inputFormat)
} // namespace Impl

CREATE_MAPPER_H(RawStorageFormat, DXGI_FORMAT);
// CREATE_MAPPER_H(DXGI_FORMAT, RawStorageFormat);

CREATE_MAPPER_H(BufferUsageRate, D3D12_INPUT_CLASSIFICATION);
CREATE_MAPPER_H(ImageType, D3D12_RESOURCE_DIMENSION);
CREATE_MAPPER_H(ImageViewType, D3D12_SRV_DIMENSION);
CREATE_MAPPER_H(ImageViewType, D3D12_DSV_DIMENSION);
CREATE_MAPPER_H(ImageViewType, D3D12_RTV_DIMENSION);
CREATE_MAPPER_H(CullMode, D3D12_CULL_MODE);
CREATE_MAPPER_H(TextureAddressMode, D3D12_TEXTURE_ADDRESS_MODE);
CREATE_MAPPER_H(TextureFilter, D3D12_FILTER);

} // namespace D3D12
} // namespace Azura