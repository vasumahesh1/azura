#pragma once

#include "Types.h"

namespace Azura {
constexpr SizeType DEFAULT_SEED{2166136261};
constexpr SizeType DEFAULT_PRIME{0x01000193};

constexpr SizeType FNVHash(const void* bytes, U32 numBytes, SizeType seed, SizeType prime) {
  const auto* ptr = static_cast<const unsigned char*>(bytes);

  SizeType hashValue = seed;

  for (auto itr = 0U; itr < numBytes; itr++) {
    hashValue = (ptr[itr] ^ hashValue) * prime;
  }

  return hashValue;
}

#define HASH(str) FNVHash(str, sizeof(str), DEFAULT_SEED, DEFAULT_PRIME)

class HashOperator {
 public:
  virtual ~HashOperator() = default;
  HashOperator()          = default;

  HashOperator(const HashOperator& other)     = default;
  HashOperator(HashOperator&& other) noexcept = default;
  HashOperator& operator=(const HashOperator& other) = default;
  HashOperator& operator=(HashOperator&& other) noexcept = default;

  HashOperator(SizeType seed);
  virtual SizeType evaluate(const void* bytes, U32 numBytes) const = 0;
  virtual SizeType evaluate(const char* bytes, U32 len) const      = 0;
  virtual SizeType evaluate(const int& num) const                  = 0;

 protected:
   SizeType mSeed{2166136261};
};

class FNVHashOperator : public HashOperator {
  SizeType mPrime{0x01000193};  // 16777619
 public:
  FNVHashOperator() = default;
  FNVHashOperator(SizeType seed);
  FNVHashOperator(SizeType seed, SizeType prime);
  SizeType evaluate(const void* bytes, U32 numBytes) const override;
  SizeType evaluate(const char* bytes, U32 len) const override;
  SizeType evaluate(const int& num) const override;
};
}  // namespace Azura
