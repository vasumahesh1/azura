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

} // namespace D3D12
} // namespace Azura