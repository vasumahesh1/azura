#pragma once

#include "Types.h"

namespace Azura {
constexpr U32 DEFAULT_SEED{2166136261};
constexpr U32 DEFAULT_PRIME{0x01000193};

constexpr U32 FNVHash(const void* bytes, U32 numBytes, U32 seed, U32 prime) {
  const auto* ptr = static_cast<const unsigned char*>(bytes);

  U32 hashValue = seed;

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

  HashOperator(U32 seed);
  virtual U32 evaluate(const void* bytes, U32 numBytes) const = 0;
  virtual U32 evaluate(const char* bytes, U32 len) const      = 0;
  virtual U32 evaluate(const int& num) const                  = 0;

 protected:
  U32 mSeed{2166136261};
};

class FNVHashOperator : public HashOperator {
  U32 mPrime{0x01000193};  // 16777619
 public:
  FNVHashOperator() = default;
  FNVHashOperator(U32 seed);
  FNVHashOperator(U32 seed, U32 prime);
  U32 evaluate(const void* bytes, U32 numBytes) const override;
  U32 evaluate(const char* bytes, U32 len) const override;
  U32 evaluate(const int& num) const override;
};
}  // namespace Azura
