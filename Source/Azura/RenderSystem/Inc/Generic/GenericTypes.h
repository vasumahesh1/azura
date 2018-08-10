#pragma once

namespace Azura {
enum class RS {
  Success = 0,
  UnknownError,
};

enum class ColorSpace {
  SRGB,
  HDR10
};

enum class PresentModes
{
  Immediate,
  Mailbox,
  FIFO,
  FIFORelaxed,
  SharedDemandRefesh,
  SharedContinuous
};
} // namespace Azura
