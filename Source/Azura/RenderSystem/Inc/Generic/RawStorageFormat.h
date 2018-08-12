#pragma once
#include <Types.h>
#include <tuple>

#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <map>

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

#define TUPLE_ACCESS_ENUM 0
#define TUPLE_ACCESS_CHANNEL_DATA 1

#define TUPLE_SIZE 2

#define RAW_STORAGE_FORMAT_ITERATOR(FUNC)                                                                                                                           \
  FUNC(( UNKNOWN,  /* CHANNELS START */ (BOOST_PP_NIL) /* CHANNELS END */ ))                                                                                                                                           \
  FUNC(( R32G32B32A32_FLOAT, /* CHANNELS START */ ((Red, Float, 32), ((Blue, Float, 32), ((Green, Float, 32), ((Alpha, Float, 32), BOOST_PP_NIL)))) /* CHANNELS END */ ))


#define ENUM_NAME_ONLY_MACRO(EnumTuple) BOOST_PP_TUPLE_ELEM(TUPLE_SIZE, TUPLE_ACCESS_ENUM, EnumTuple)
#define ENUM_MACRO(EnumTuple) ENUM_NAME_ONLY_MACRO(EnumTuple),

enum class RawStorageFormat {
  RAW_STORAGE_FORMAT_ITERATOR(ENUM_MACRO)
};
} // namespace Azura
