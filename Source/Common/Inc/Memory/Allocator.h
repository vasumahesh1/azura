#pragma once

#include "Types.h"
#include "MemoryBuffer.h"
#include <type_traits>
#include <functional>

namespace Azura {
namespace Memory {
namespace Impl {
inline U32 AlignAhead(U32 size, U32 alignment) {
  return (size + (alignment - 1)) & ~(alignment - 1);
}
} // namespace Impl

template <typename T>
using UniquePtr = std::unique_ptr<T, std::function<void(T*)>>;

template <typename T>
using UniqueArrayPtr = std::unique_ptr<T[], std::function<void(T*)>>;

struct MemoryRange {
  U32 m_offset;
  U32 m_size;

  MemoryRange(U32 offset, U32 size);
};


class Allocator {
public:
  Allocator(void* resource, U32 size);
  Allocator(AddressPtr resource, U32 size);
  virtual ~Allocator();

  // Disable Copy
  Allocator(const Allocator& other) = delete;
  Allocator& operator=(const Allocator& other) = delete;

  // Enable Move
  Allocator(Allocator&& other) noexcept = default;
  Allocator& operator=(Allocator&& other) noexcept = default;

  template <typename Type, typename... Args>
  UniquePtr<Type> New(Args ... args);

  template <typename Type, typename ...Args>
  UniqueArrayPtr<Type> NewArray(U32 numElements, Args ...args);

  template <typename Type, typename... Args>
  UniquePtr<Type> RawNew(Args ... args);

  template <typename Type, typename ...Args>
  UniqueArrayPtr<Type> RawNewArray(U32 numElements, Args ...args);

#ifdef BUILD_UNIT_TEST
  AddressPtr GetBasePtr() const { return BasePtr(); };
  AddressPtr GetSize() const { return Size(); };
#endif

protected:
  U32 Size() const;
  AddressPtr BasePtr() const;

  virtual void* Allocate(U32 size, U32 alignment) = 0;
  virtual void Deallocate(void* address) = 0;
  virtual void Reset();

private:
  template <typename Type, bool Construct, typename... Args>
  UniquePtr<Type> InternalAllocate(U32 size, U32 alignment, Args ... args);
  template <class Type, bool Construct, class ... Args>
  UniqueArrayPtr<Type> InternalAllocateArray(U32 elementSize,
                                             U32 numElements,
                                             U32 alignment,
                                             Args ... args);

  AddressPtr m_basePtr;
  U32 m_size;
};

template <typename Type, typename... Args>
UniquePtr<Type> Allocator::New(Args ... args) {
  return InternalAllocate<Type, true, Args...>(sizeof(Type), sizeof(Type), args...);
}

template <typename Type, typename... Args>
UniqueArrayPtr<Type> Allocator::NewArray(U32 numElements, Args ... args) {
  return InternalAllocateArray<Type, true, Args...>(sizeof(Type), numElements, sizeof(Type), args...);
}

template <typename Type, typename ... Args>
UniquePtr<Type> Allocator::RawNew(Args ... args) {
  return InternalAllocate<Type, false, Args...>(sizeof(Type), sizeof(Type), args...);
}

template <typename Type, typename... Args>
UniqueArrayPtr<Type> Allocator::RawNewArray(U32 numElements, Args ... args) {
  return InternalAllocateArray<Type, false, Args...>(sizeof(Type), numElements, sizeof(Type), args...);
}

template <typename Type, bool Construct, typename ... Args>
UniquePtr<Type> Allocator::InternalAllocate(U32 size, U32 alignment, Args ... args) {
  Type* address = reinterpret_cast<Type*>(Allocate(size, alignment));

  // Allocator couldn't allocate
  if (address == nullptr) {
    return nullptr;
  }

  if constexpr (Construct) {
    new(address) Type(args...);
  }

  const auto customDeleter = [&](Type* data)
  {
    data->~Type();
    Deallocate(data);
  };

  UniquePtr<Type> result(address, customDeleter);
  return result;
}

template <typename Type, bool Construct, typename ... Args>
UniqueArrayPtr<Type> Allocator::InternalAllocateArray(U32 elementSize,
                                                      U32 numElements,
                                                      U32 alignment,
                                                      Args ... args) {
  const U32 actualSize = Impl::AlignAhead(elementSize, alignment);
  Type* address        = reinterpret_cast<Type*>(Allocate(actualSize * numElements, alignment));

  // Allocator couldn't allocate
  if (address == nullptr) {
    return nullptr;
  }

  if constexpr (Construct) {
    for (U32 idx = 0; idx < numElements; ++idx) {
      new(address + (idx * actualSize)) Type(args...);
    }
  }

  const auto customDeleter = [&](Type* data)
  {
    // if constexpr (Construct) {
    //   Type* ptr    = data;
    //   for (U32 idx = 0; idx < numElements; ++idx) {
    //     ptr->~Type();
    //     ++ptr;
    //   }
    // }

    Deallocate(data);
  };

  UniqueArrayPtr<Type> result(address, customDeleter);
  return result;
}

} // namespace Memory
} // namespace Azura
