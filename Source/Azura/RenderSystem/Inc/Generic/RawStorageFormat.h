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

// #define ARR(...) __VA_ARGS__
//
// // NOLINTNEXTLINE
// #define Channel(ChannelEnum, ChannelType, BitWidth) ChannelDesc(Channel::##ChannelEnum, ChannelDataType::##ChannelType, BitWidth)
//
// #define RAW_STORAGE_FORMAT_ITERATOR(FUNC)                                                                                                                           \
//   FUNC(UNKNOWN, ARR({}), 0, 0)                                                                                                                                           \
//   FUNC(R32G32B32A32_FLOAT, ARR({Channel(Red, Float, Bits32), Channel(Green, Float, Bits32), Channel(Blue, Float, Bits32), Channel(Alpha, Float, Bits32)}), 128, 0)
//
// // NOLINTNEXTLINE
// #define RAW_STORAGE_ENUM(Enum, Channels, TotalBitWidth, FeatureFlags) AZ_##Enum,
//
// // NOLINTNEXTLINE
// enum class RawStorageFormat {
//   // NOLINTNEXTLINE
//   RAW_STORAGE_FORMAT_ITERATOR(RAW_STORAGE_ENUM)
// };


// #define RAW_STORAGE_FORMAT(I) RAW_STORAGE_FORMAT ## I
//
// #define RAW_STORAGE_FORMAT0 (UNKNOWN, 0)
// #define RAW_STORAGE_FORMAT1 (R32G32B32A32_FLOAT, 128)
// #define RAW_STORAGE_FORMAT2 (B8G8R8A8_UNORM, 32)
//
// #define RAW_STORAGE_FORMAT_CNT 3
//
// #define ENUM_MACRO(z, I, _) BOOST_PP_TUPLE_ELEM(2, 0, RAW_STORAGE_FORMAT(I)),
//
// enum class RawStorageFormat {
//   BOOST_PP_REPEAT(RAW_STORAGE_FORMAT_CNT, ENUM_MACRO, _)
// };

#define TUPLE_ACCESS_ENUM 0
#define TUPLE_ACCESS_CHANNEL_DATA 1

#define TUPLE_SIZE 2

#define RAW_STORAGE_FORMAT_ITERATOR(FUNC)                                                                                                                           \
  FUNC(( UNKNOWN,  /* CHANNELS START */ (BOOST_PP_NIL) /* CHANNELS END */ ))                                                                                                                                           \
  FUNC(( R32G32B32A32_FLOAT, /* CHANNELS START */ ((Red, Float, 32), ((Blue, Float, 32), ((Green, Float, 32), ((Alpha, Float, 32), BOOST_PP_NIL)))) /* CHANNELS END */ ))


#define ENUM_NAME_ONLY_MACRO(EnumTuple) BOOST_PP_CAT(AZ_, BOOST_PP_TUPLE_ELEM(TUPLE_SIZE, TUPLE_ACCESS_ENUM, EnumTuple))
#define ENUM_MACRO(EnumTuple) ENUM_NAME_ONLY_MACRO(EnumTuple),

enum class RawStorageFormat {
  RAW_STORAGE_FORMAT_ITERATOR(ENUM_MACRO)
};

struct FormatDesc
{
  U32 m_size;

  FormatDesc(U32 size): m_size(size)
  {
    
  }
};

const RawStorageFormat TEST_VALUE = RawStorageFormat::AZ_R32G32B32A32_FLOAT;


#define CHANNEL_SIZE_ITR(r, data, elem) BOOST_PP_CAT(elem, data)
  //BOOST_PP_TUPLE_ELEM(3, 2, elem)

#define LIST (0, (1, (2, (3, BOOST_PP_NIL))))
//
// #define FORMAT_DESC_PAIR(EnumTuple) std::make_pair(RawStorageFormat::ENUM_NAME_ONLY_MACRO(EnumTuple), FormatDesc( 0)),
// std::map<RawStorageFormat, FormatDesc> test = {
//   RAW_STORAGE_FORMAT_ITERATOR(FORMAT_DESC_PAIR)
// };

// BOOST_PP_LIST_FOR_EACH(CHANNEL_SIZE_ITR, +, LIST)
//
// const U32 test2 = 

// ENUM_NAME_ONLY_MACRO(EnumTuple)

// BOOST_PP_LIST_FOR_EACH(CHANNEL_SIZE_ITR, +, BOOST_PP_TUPLE_ELEM(TUPLE_SIZE, TUPLE_ACCESS_CHANNEL_DATA, EnumTuple)

} // namespace Azura
