#include "Memory/Allocator.h"

namespace Azura {
namespace Memory {

MemoryRange::MemoryRange(U32 offset, U32 size) : m_offset(offset), m_size(size) {}

Allocator::Allocator(void* resource, U32 size)
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    : m_basePtr(reinterpret_cast<AddressPtr>(resource)), m_size(size) {}

Allocator::Allocator(AddressPtr resource, U32 size) : m_basePtr(resource), m_size(size) {}

Allocator::~Allocator() = default;

void Allocator::Reset() {}

U32 Allocator::Size() const {
  return m_size;
}

AddressPtr Allocator::BasePtr() const {
  return m_basePtr;
}
}  // namespace Memory
}  // namespace Azura
