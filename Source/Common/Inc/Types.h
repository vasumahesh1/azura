#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace Azura {
using UINT = unsigned int;
using U8 = std::uint8_t;
using U16 = std::uint16_t;
using U32 = std::uint32_t;
using U64 = std::uint64_t;
using UPTR = std::uintptr_t;
using SizeType = std::size_t;
using String = std::string;
using AddressPtr = UPTR;
using AddressDiff = ptrdiff_t;
} // namespace Azura
