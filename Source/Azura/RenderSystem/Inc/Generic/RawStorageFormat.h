#pragma once
#include <Types.h>
#include <tuple>

namespace Azura {
enum Bits {
  BitsUnknown = 0,
  Bits1 = 1,
  Bits2 = 2,
  Bits3 = 3,
  Bits4 = 4,
  Bits5 = 5,
  Bits6 = 6,
  Bits7 = 7,
  Bits8 = 8,
  Bits16 = 16,
  Bits32 = 32
};

enum FormatFeature : U32 {
  Compressed = 1,
  MultiBlock = 1 << 1,
};

enum class ChannelDataType {
  Float = 0,
  Integer,
  UNorm,
  SNorm,
  UScaled,
  SScaled
};

enum class Channel {
  Red = 0,
  Green,
  Blue,
  Alpha,
  Depth,
  Stencil,
  Padding,
};

using ChannelDesc = std::tuple<Channel, ChannelDataType, U32>;

#define ARR(...) __VA_ARGS__

// NOLINTNEXTLINE
#define Channel(ChannelEnum, ChannelType, BitWidth) ChannelDesc(Channel::##ChannelEnum, ChannelDataType::##ChannelType, BitWidth)

#define RAW_STORAGE_FORMAT_ITERATOR(FUNC)                                                                                                                           \
  FUNC(UNKNOWN, ARR({}), 0, 0)                                                                                                                                           \
  FUNC(R32G32B32A32_FLOAT, ARR({Channel(Red, Float, Bits32), Channel(Green, Float, Bits32), Channel(Blue, Float, Bits32), Channel(Alpha, Float, Bits32)}), 128, 0)

// NOLINTNEXTLINE
#define RAW_STORAGE_ENUM(Enum, Channels, TotalBitWidth, FeatureFlags) AZ_##Enum,

// NOLINTNEXTLINE
enum class RawStorageFormat {
  // NOLINTNEXTLINE
  RAW_STORAGE_FORMAT_ITERATOR(RAW_STORAGE_ENUM)
};

} // namespace Azura
