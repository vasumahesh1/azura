#pragma once
#include <Types.h>
#include <tuple>

#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

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
  SScaled,
  Typeless
};

enum class Channel {
  Red = 0,
  Green,
  Blue,
  Alpha,
  Depth,
  Stencil,
  Padding,
  Unknown,
};

using ChannelDesc = std::tuple<Channel, ChannelDataType, U32>;

// Tuple Access Info
#define TUPLE_ACCESS_ENUM 0
#define TUPLE_ACCESS_CHANNEL_DATA 1
#define TUPLE_SIZE 2

#define CHANNEL_TUPLE_ACCESS_CHANNEL 0
#define CHANNEL_TUPLE_ACCESS_TYPE 1
#define CHANNEL_TUPLE_ACCESS_BIT_WIDTH 2
#define CHANNEL_TUPLE_SIZE 3

// Format Iterator
#define RAW_STORAGE_FORMAT_ITERATOR(FUNC)                                                                                                                                   \
  FUNC(( UNKNOWN,                   /* CHANNELS START */ ((Unknown, Typeless, 0))                                                             /* CHANNELS END */ ))         \
  FUNC(( R32G32B32A32_FLOAT,        /* CHANNELS START */ ((Red, Float, 32)) ((Blue, Float, 32)) ((Green, Float, 32)) ((Alpha, Float, 32))     /* CHANNELS END */ ))

// Macros For: ENUM NAME
#define GET_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE) BOOST_PP_TUPLE_ELEM(TUPLE_SIZE, TUPLE_ACCESS_ENUM, ENUM_TUPLE)
#define GET_FORMAT_NAME_FOR_LIST_FROM_TUPLE(ENUM_TUPLE) GET_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE),
#define GET_FULL_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE) RawStorageFormat::GET_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE)

// Macros For: CHANNEL WIDTH
#define GET_CHANNEL_WIDTH(CHANNEL_TUPLE) BOOST_PP_TUPLE_ELEM(CHANNEL_TUPLE_SIZE, CHANNEL_TUPLE_ACCESS_BIT_WIDTH, CHANNEL_TUPLE)
#define GET_CHANNEL_WIDTH_SEQ(R, DATA, CHANNEL_TUPLE) BOOST_PP_TUPLE_ELEM(CHANNEL_TUPLE_SIZE, CHANNEL_TUPLE_ACCESS_BIT_WIDTH, CHANNEL_TUPLE) DATA
#define GET_FORMAT_WIDTH(CHANNEL_LIST) BOOST_PP_SEQ_FOR_EACH(GET_CHANNEL_WIDTH_SEQ, +, CHANNEL_LIST) 0

#define GET_FORMAT_WIDTH_FROM_TUPLE(ENUM_TUPLE) GET_FORMAT_WIDTH(BOOST_PP_TUPLE_ELEM(TUPLE_SIZE, TUPLE_ACCESS_CHANNEL_DATA, ENUM_TUPLE))
#define GET_FORMAT_WIDTH_FOR_LIST_FROM_TUPLE(ENUM_TUPLE) GET_FORMAT_WIDTH_FROM_TUPLE(ENUM_TUPLE),


/**
 * \brief This enum represents a platform agnostic way of representing Raw Data.
 * It can be used in Images, Image Views, Buffers etc.
 */
enum class RawStorageFormat {
  RAW_STORAGE_FORMAT_ITERATOR(GET_FORMAT_NAME_FOR_LIST_FROM_TUPLE)
};

U32 GetFormatSize(RawStorageFormat format);

} // namespace Azura
