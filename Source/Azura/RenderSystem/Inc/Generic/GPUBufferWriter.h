#pragma once

#include "Generic/GPUBuffer.h"

namespace Azura {
class Log;

namespace GPUBufferWriter {
U32 AppendRaw(MappedGPUBuffer& buffer,
              DescriptorType type,
              void* data,
              U32 byteSize,
              U32 alignment,
              const Log& log);

U32 AppendRaw(MappedGPUBuffer& buffer,
              DescriptorType type,
              void* data,
              U32 byteSize,
              U32 maxSize,
              U32 alignment,
              const Log& log);

U32 AppendTexture(MappedGPUBuffer& buffer,
                  void* data,
                  U32 byteSize,
                  U32 alignment,
                  U32 currentRowPitch,
                  U32 requiredRowPitch,
                  const Log& log);
};

} // namespace Azura
