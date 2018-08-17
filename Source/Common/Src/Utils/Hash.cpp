#include "Utils/Hash.h"

namespace Azura {
HashOperator::HashOperator(U32 seed)
  : mSeed(seed) {
}

FNVHashOperator::FNVHashOperator(U32 seed)
  : HashOperator(seed) {
}

FNVHashOperator::FNVHashOperator(U32 seed, U32 prime)
  : HashOperator(seed),
    mPrime(prime) {
}

U32 FNVHashOperator::evaluate(const void* bytes, U32 numBytes) const {
  return FNVHash(bytes, numBytes, mSeed, mPrime);
}

U32 FNVHashOperator::evaluate(const char* bytes, U32 len) const {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return evaluate(reinterpret_cast<const void*>(bytes), len);
}

U32 FNVHashOperator::evaluate(const int& num) const {
  return evaluate(&num, sizeof(int));
}
} // namespace Azura
