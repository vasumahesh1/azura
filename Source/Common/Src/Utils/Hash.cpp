#include "Utils/Hash.h"

namespace Azura {
HashOperator::HashOperator(SizeType seed) : mSeed(seed) {}

FNVHashOperator::FNVHashOperator(SizeType seed) : HashOperator(seed) {}

FNVHashOperator::FNVHashOperator(SizeType seed, SizeType prime) : HashOperator(seed), mPrime(prime) {}

SizeType FNVHashOperator::evaluate(const void* bytes, U32 numBytes) const {
  return FNVHash(bytes, numBytes, mSeed, mPrime);
}

SizeType FNVHashOperator::evaluate(const char* bytes, U32 len) const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return evaluate(reinterpret_cast<const void*>(bytes), len);
}

SizeType FNVHashOperator::evaluate(const int& num) const {
  return evaluate(&num, sizeof(int));
}
}  // namespace Azura
