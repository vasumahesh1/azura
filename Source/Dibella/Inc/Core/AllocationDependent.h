#pragma once

namespace Dibella {
class Allocator;

class AllocationDependent {
 protected:
  explicit AllocationDependent(Allocator* alloc);
  Allocator* mAllocator;
};

inline AllocationDependent::AllocationDependent(Allocator* alloc) : mAllocator(alloc) {}
}  // namespace Dibella
