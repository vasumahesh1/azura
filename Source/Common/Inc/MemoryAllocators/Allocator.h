#pragma once

#include "Types.h"
#include "MemoryBuffer.h"
#include <type_traits>

namespace Azura {
  constexpr SizeType DEFAULT_MEMORY_ALIGNMENT = 16;

  template<typename T, SizeType A>
  struct AlignedType final
  {
    AlignedType() = delete;

    using Type = T;
    static const SizeType sAlignment = A;
  };

  template<typename>
  struct is_aligned_type : std::false_type {};

  template<typename Type, SizeType Alignment>
  struct is_aligned_type<AlignedType<Type, Alignment>> : std::true_type {};

  class Allocator {
  public:
    Allocator();
    virtual ~Allocator();

    Allocator(const Allocator& other) = delete;
    Allocator& operator=(const Allocator& other) = delete;

    Allocator(Allocator&& other) noexcept = default;
    Allocator& operator=(Allocator&& other) noexcept = default;

    virtual void* Allocate(SizeType size, SizeType alignment) = 0;
    virtual void Deallocate(void* address) = 0;
    virtual void Reset();

    template <class Type>
    void DeleteObject(Type* object);

    template <class Type>
    void DeleteObjects(Type* object, UINT count);

    template <typename Type, typename... Args, typename = std::enable_if_t<!is_aligned_type<Type>::value>>
    Type* NewObject(bool construct, Args ... args) {
      Type* address = reinterpret_cast<Type*>(Allocate(sizeof(Type), sizeof(Type)));

      if (construct) {
        new (address) Type(args...);
      }

      return address;
    }

     template <typename AlignedType, typename... Args, typename = std::enable_if_t<is_aligned_type<AlignedType>::value>>
     typename AlignedType::Type* NewObject(bool construct, Args ... args) {
       typename AlignedType::Type* address = reinterpret_cast<typename AlignedType::Type*>(Allocate(sizeof(typename AlignedType::Type), AlignedType::sAlignment));
    
       if (construct) {
         new (address) typename AlignedType::Type(args...);
       }
    
       return address;
     }

    template <typename Type, typename... Args, typename = std::enable_if_t<!is_aligned_type<Type>::value>>
    Type* NewObjects(const UINT count, bool construct) {
      Type* address = reinterpret_cast<Type*>(Allocate(count * sizeof(Type), sizeof(Type)));
      Type* start = address;

      if (construct) {
        for (auto itr = 0U; itr < count; ++itr) {
          new (start) Type();
          ++start;
        }
      }

      return address;
    }

     template <typename AlignedType, typename... Args, typename = std::enable_if_t<is_aligned_type<AlignedType>::value>>
     typename AlignedType::Type* NewObjects(const UINT count, bool construct) {
       typename AlignedType::Type* address = reinterpret_cast<Type*>(Allocate(count * sizeof(typename AlignedType::Type), AlignedType::sAlignment));
       typename AlignedType::Type* start = address;
    
       if (construct) {
         for (auto itr = 0U; itr < count; ++itr) {
           new (start) typename AlignedType::Type();
           ++start;
         }
       }
    
       return address;
     }
  };

  template <typename Type>
  void Allocator::DeleteObject(Type* object) {
    // TODO: Figure out Dynamic Type based destruction.
    void* address = static_cast<void*>(object);
    object->~Type();
    Deallocate(address);
  }

  template <typename Type>
  void Allocator::DeleteObjects(Type* object, const UINT count) {
    void* address = static_cast<void*>(object);
    Type* start   = object;

    for (auto itr = 0U; itr < count; ++itr) {
      start->~Type();
      ++start;
    }

    // TODO: Figure out Deallocate for Arrays
    Deallocate(address);
  }  
} // namespace Azura
