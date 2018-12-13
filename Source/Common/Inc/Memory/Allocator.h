#pragma once

#include <functional>
#include "Types.h"
#include "Utils/Macros.h"

namespace Azura {
namespace Memory {
template <typename T>
using UniquePtr = std::unique_ptr<T, std::function<void(T*)>>;

template <typename T>
using UniqueArrayPtr = std::unique_ptr<T[], std::function<void(T*)>>;

struct MemoryRange {
  U32 m_offset;
  U32 m_size;

  MemoryRange(U32 offset, U32 size);
};

/**
 * \brief Base class for an Allocator
 * \ingroup Common
 */
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

  /**
   * \brief Allocates & initializes the Type and returns a pointer to that memory location
   * \details New is analogous to the C++ new keyword. It tells the allocator to construct an object in the memory recourse it is managing.
   * This functions first allocates a block of memory and then constructs it with the supplied params.
   * Some example usages:
   * \code{.cpp}
   * auto floatDataPtr = MyAllocator.New<float>(0.0f);
   * 
   * class CustomClass{};
   * auto customDataPtr = MyAllocator.New<CustomClass>();
   * \endcode
   * 
   * The actual returned type is a `UniquePtr<T>` which is a std::unique_ptr with a custom deleter.
   *
   * \tparam Type Data Type to allocate
   * \tparam Args Initialization Arguments
   * \param args The list of arguments that Type needs for construction
   *
   * \warning Can return a nullptr in case of failed Allocation
   *
   * \return A unique pointer with a custom deleter
   */
  template <typename Type, typename... Args>
  UniquePtr<Type> New(Args ... args);

  template <typename Type, typename... Args>
  UniqueArrayPtr<Type> NewArray(U32 numElements, Args ... args);

  template <typename Type, typename... Args>
  UniquePtr<Type> RawNew(Args ... args);

  template <typename Type, typename... Args>
  UniqueArrayPtr<Type> RawNewArray(U32 numElements, Args ... args);

#ifdef BUILD_UNIT_TEST
  AddressPtr GetBasePtr() const {
    return BasePtr();
  };
  AddressPtr GetSize() const {
    return Size();
  };
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
  template <class Type, bool Construct, class... Args>
  UniqueArrayPtr<Type> InternalAllocateArray(U32 elementSize, U32 numElements, U32 alignment, Args ... args);

  AddressPtr m_basePtr;
  U32 m_size;
};

template <typename Type, typename... Args>
UniquePtr<Type> Allocator::New(Args ... args) {
  return InternalAllocate<Type, true, Args...>(sizeof(Type), alignof(Type), args...);
}

template <typename Type, typename... Args>
UniqueArrayPtr<Type> Allocator::NewArray(U32 numElements, Args ... args) {
  return InternalAllocateArray<Type, true, Args...>(sizeof(Type), numElements, alignof(Type), args...);
}

template <typename Type, typename... Args>
UniquePtr<Type> Allocator::RawNew(Args ... args) {
  return InternalAllocate<Type, false, Args...>(sizeof(Type), alignof(Type), args...);
}

template <typename Type, typename... Args>
UniqueArrayPtr<Type> Allocator::RawNewArray(U32 numElements, Args ... args) {
  return InternalAllocateArray<Type, false, Args...>(sizeof(Type), numElements, alignof(Type), args...);
}

template <typename Type, bool Construct, typename... Args>
UniquePtr<Type> Allocator::InternalAllocate(U32 size, U32 alignment, Args ... args) {
  Type* address = reinterpret_cast<Type*>(Allocate(size, alignment));

  // Allocator couldn't allocate
  if (address == nullptr) {
    return nullptr;
  }

  if constexpr (Construct) {
    new(address) Type(args...);
  }

  const auto customDeleter = [this](Type* data)
  {
    data->~Type();
    Deallocate(data);
  };

  UniquePtr<Type> result(address, customDeleter);
  return result;
}

template <typename Type, bool Construct, typename... Args>
UniqueArrayPtr<Type> Allocator::InternalAllocateArray(U32 elementSize, U32 numElements, U32 alignment, Args ... args) {
  Type* address = reinterpret_cast<Type*>(Allocate(elementSize * numElements, alignment));

  // Allocator couldn't allocate
  if (address == nullptr) {
    return nullptr;
  }

  if constexpr (Construct) {
    for (U32 idx = 0; idx < numElements; ++idx) {
      new(address + (idx * elementSize)) Type(args...);
    }
  }

  const auto customDeleter = [this, numElements](Type* data)
  {
    if constexpr (Construct) {
      Type* ptr    = data;
      for (U32 idx = 0; idx < numElements; ++idx) {
        UNUSED(ptr->~Type());
        ++ptr;
      }
    }

    Deallocate(data);
  };

  UniqueArrayPtr<Type> result(address, customDeleter);
  return result;
}

} // namespace Memory
} // namespace Azura
