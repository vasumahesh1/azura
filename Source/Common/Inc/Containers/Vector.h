#pragma once

// for memcpy
#include <cassert>
#include <cstring>
#include <functional>
#include <iterator>

#include "Array.h"
#include "Memory/Allocator.h"
#include "Types.h"

#include "Utils/Macros.h"

namespace Azura {
namespace Containers {
struct ContainerExtent {
  U32 m_size;
  U32 m_reserveSize;

  explicit ContainerExtent(const U32 size)
    : m_size(size),
      m_reserveSize(size) {
  }

  ContainerExtent(const U32 size, const U32 reserveSize)
    : m_size(size),
      m_reserveSize(reserveSize) {
  }
};

template <typename Type>
class Vector {
public:
  Vector() = default;
  explicit Vector(Memory::Allocator& alloc);

  Vector(UINT maxSize, Memory::Allocator& alloc);

  Vector(const std::initializer_list<Type>& list, Memory::Allocator& alloc);

  template <typename... Args>
  Vector(ContainerExtent extent, Memory::Allocator& alloc, Args&&... args);

  ~Vector();

  // Copy Ctor
  Vector(const Vector& other);

  // Move Ctor
  Vector(Vector&& other) noexcept;

  Vector& operator=(const Vector& other);
  Vector& operator=(Vector&& other) noexcept;

  /**
   * \brief Appends data to the end of the vector
   * \param data Data to push
   */
  void PushBack(const Type& data);

  /**
   * \brief Appends data to the end of the vector
   * \param data Data to push
   */
  void PushBack(Type&& data);

  /**
   * \brief Appends data to the end of the vector
   * \param args Arguments for emplacement
   */
  template <typename... Args>
  void EmplaceBack(Args ... args);

  /**
   * \brief Removes the last element in the vector array
   */
  void PopBack();

  /**
   * \brief Searches for the data in the Vector
   * \param data Data to search for
   * \return Index if Found, else -1
   */
  int FindFirst(const Type& data);

  /**
   * \brief Searches for the given data in the vector and removes it
   * \param data Data to Search for and Remove
   */
  void Remove(const Type& data);

  /**
   * \brief Reserves a max contiguous block for the vector
   * Use only when you didn't supply a maxSize in the constructor.
   * \param maxSize Maxium Possible Size
   */
  void Reserve(U32 maxSize);
  void Resize(U32 maxSize);

  /**
   * \brief Checks if the container is empty
   * \return true if empty
   */
  bool IsEmpty() const;

  /**
   * \brief Inserts the supplied data at the index
   * \param idx Target Index
   * \param data Data to insert
   */
  void InsertAt(U32 idx, const Type& data);

  /**
   * \brief Gets the Data pointer
   */
  Type* Data();

  const Type* Data() const;

  Type& operator[](U32 idx);
  Type& operator[](U32 idx) const;

  U32 GetSize() const {
    return m_size;
  }

  U32 GetMaxSize() const {
    return m_maxSize;
  }

  template <class InputIt>
  void Assign(InputIt first, InputIt last);

  class Iterator {
  public:
    Iterator() = default;
    ~Iterator() = default;
    Iterator(const Iterator& other) = default;
    Iterator& operator=(const Iterator& other) = default;

    using iterator_category = std::random_access_iterator_tag;
    using value_type = Type;
    using difference_type = int;
    using pointer = Type*;
    using reference = Type&;

    Iterator(const Vector* ptr, int index)
      : mPtr(ptr),
        mIndex(index) {
    }

    Iterator(Iterator&& other) noexcept = default;
    Iterator& operator=(Iterator&& other) noexcept = default;

    // Pre Increment
    Iterator& operator++() {
      ++mIndex;
      return *this;
    }

    // Post Increment
    Iterator operator++(int) {
      Iterator copy(*this);
      operator++();
      return copy;
    }

    // Pre Decrement
    Iterator& operator--() {
      --mIndex;
      return *this;
    }

    // Post Decrement
    Iterator operator--(int) {
      Iterator copy(*this);
      operator--();
      return copy;
    }

    bool operator==(const Iterator& rhs) {
      return mPtr == rhs.mPtr && mIndex == rhs.mIndex;
    }

    bool operator!=(const Iterator& rhs) {
      return !(*this == rhs);
    }

    bool operator<(const Iterator& rhs) {
      assert(mPtr == rhs.mPtr);
      return mIndex < rhs.mIndex;
    }

    bool operator<=(const Iterator& rhs) {
      assert(mPtr == rhs.mPtr);
      return mIndex <= rhs.mIndex;
    }

    bool operator>(const Iterator& rhs) {
      assert(mPtr == rhs.mPtr);
      return mIndex > rhs.mIndex;
    }

    bool operator>=(const Iterator& rhs) {
      assert(mPtr == rhs.mPtr);
      return mIndex >= rhs.mIndex;
    }

    Iterator operator+(const int& idx) {
      return Iterator(mPtr, mIndex + idx);
    }

    Iterator& operator+=(const int& idx) {
      mIndex += idx;
      return *this;
    }

    Iterator operator-(const int& idx) {
      assert(mIndex - idx > 0);
      return Iterator(mPtr, mIndex - idx);
    }

    Iterator& operator-=(const int& idx) {
      assert(mIndex - idx > 0);
      mIndex -= idx;
      return *this;
    }

    int operator-(const Iterator& rhs) {
      assert(mPtr == rhs.mPtr);
      return mIndex - rhs.mIndex;
    }

    friend int operator-(const Iterator& lhs, const Iterator& rhs) {
      Iterator copy(lhs);
      return copy - rhs;
    }

    // Element Accessors

    Type& operator*() {
      return mPtr->operator[](mIndex);
    }

    Type* operator->() {
      return &mPtr->operator[](mIndex);
    }

    Type& operator[](const int& idx) {
      return mPtr->operator[](mIndex + idx);
    }

  private:
    const Vector<Type>* mPtr{nullptr};
    int mIndex{-1};
  };

  /**
   * \brief Returns an Iterator pointing to the beginning of the vector. This is similar to begin() of a std::vector.
   * \return Iterator
   */
  Iterator Begin() const;

  /**
   * \brief Returns an Iterator pointing to the end of the vector. This is similar to end() of a std::vector.
   * \return Iterator
   */
  Iterator End() const;

  /**
   * \brief Returns an Iterator pointing to the beginning of the vector. This is similar to begin() of a std::vector.
   * \return Iterator
   */
  Iterator begin() const;

  /**
   * \brief Returns an Iterator pointing to the end of the vector. This is similar to end() of a std::vector.
   * \return Iterator
   */
  Iterator end() const;

private:
  U32 m_size{0};
  U32 m_maxSize{0};
  std::reference_wrapper<Memory::Allocator> m_allocator;
  Memory::UniqueArrayPtr<Type> m_base{nullptr};
};

template <typename Type>
Vector<Type>::Vector(Memory::Allocator& alloc)
  : m_allocator(alloc) {
}

template <typename Type>
Vector<Type>::Vector(const UINT maxSize, Memory::Allocator& alloc)
  : m_maxSize(maxSize),
    m_allocator(alloc),
    m_base(m_allocator.get().RawNewArray<Type>(m_maxSize)) {
}

template <typename Type>
Vector<Type>::Vector(const std::initializer_list<Type>& list, Memory::Allocator& alloc)
  : m_size(U32(list.size())),
    m_maxSize(U32(list.size())),
    m_allocator(alloc),
    m_base(m_allocator.get().RawNewArray<Type>(m_maxSize)) {

  if constexpr (std::is_trivially_copyable_v<Type>) {
    // Copy over Contents
    std::memcpy(m_base.get(), list.begin(), m_size * sizeof(Type));
  } else {
    const Type* start = list.begin();
    // Manually Copy Construct each item
    for (U32 idx = 0; idx < m_size; ++idx) {
      new(&m_base[idx]) Type(*(start + idx));
    }
  }
}

template <typename Type>
template <typename... Args>
Vector<Type>::Vector(ContainerExtent extent, Memory::Allocator& alloc, Args&&... args)
  : m_maxSize(extent.m_reserveSize),
    m_size(extent.m_size),
    m_allocator(alloc),
    m_base(m_allocator.get().RawNewArray<Type>(m_maxSize)) {
  assert(m_size <= m_maxSize);

  for (U32 idx = 0; idx < m_size; ++idx) {
    new(&m_base[idx]) Type(std::forward<Args>(args)...);
  }
}

template <typename Type>
Vector<Type>::~Vector() {
  for (U32 idx = 0; idx < m_size; ++idx) {
    // TODO(vasumahesh1): MSVC gives a NoDIscard warning here. Not sure why, but due to C++17.
    UNUSED(m_base[idx].~Type());
  }
};

template <typename Type>
Vector<Type>::Vector(const Vector& other)
  : m_size(other.m_size),
    m_maxSize(other.m_maxSize),
    m_allocator(other.m_allocator) {
  // Allocate Memory
  m_base = m_allocator.get().RawNewArray<Type>(m_maxSize);

  if constexpr (std::is_trivially_copyable_v<Type>) {
    // Copy over Contents
    std::memcpy(m_base.get(), other.m_base.get(), other.m_size * sizeof(Type));
  } else {
    // Manually Copy Construct each item
    for (U32 idx = 0; idx < other.m_size; ++idx) {
      new(&m_base[idx]) Type(other.m_base[idx]);
    }
  }
}

template <typename Type>
Vector<Type>::Vector(Vector&& other) noexcept
  : m_size(std::move(other.m_size)),
    m_maxSize(std::move(other.m_maxSize)),
    m_allocator(other.m_allocator),
    m_base(std::move(other.m_base)) {
  other.m_base    = nullptr;
  other.m_size    = 0;
  other.m_maxSize = 0;
}

template <typename Type>
Vector<Type>& Vector<Type>::operator=(const Vector& other) {
  if (this == &other) {
    return *this;
  }

  m_size      = other.m_size;
  m_maxSize   = other.m_maxSize;
  m_allocator = other.m_allocator;

  // Allocate Memory
  m_base = m_allocator.get().RawNewArray<Type>(m_maxSize);

  if constexpr (std::is_trivially_copyable_v<Type>) {
    // Copy over Contents
    std::memcpy(m_base.get(), other.m_base.get(), other.m_size * sizeof(Type));
  } else {
    // Manually Copy Construct each item
    for (U32 idx = 0; idx < other.m_size; ++idx) {
      new(&m_base[idx]) Type(other.m_base[idx]);
    }
  }

  return *this;
}

template <typename Type>
Vector<Type>& Vector<Type>::operator=(Vector&& other) noexcept {
  if (this == &other) {
    return *this;
  }

  m_size      = std::move(other.m_size);
  m_maxSize   = std::move(other.m_maxSize);
  m_allocator = other.m_allocator;
  m_base      = std::move(other.m_base);

  other.m_base    = nullptr;
  other.m_size    = 0;
  other.m_maxSize = 0;

  return *this;
}

template <typename Type>
void Vector<Type>::PushBack(const Type& data) {
  assert(m_size < m_maxSize);
  new(&m_base[m_size]) Type(data);
  ++m_size;
}

template <typename Type>
void Vector<Type>::PushBack(Type&& data) {
  assert(m_size < m_maxSize);
  new(&m_base[m_size]) Type(std::move(data));
  ++m_size;
}

template <typename Type>
template <typename... Args>
void Vector<Type>::EmplaceBack(Args ... args) {
  assert(m_size < m_maxSize);
  new(&m_base[m_size]) Type(args...);
  ++m_size;
}

template <typename Type>
void Vector<Type>::PopBack() {
  assert(m_size > 0);

  const U32 targetId = m_size - 1;
  m_base[targetId].~Type();
  --m_size;
}

template <typename Type>
int Vector<Type>::FindFirst(const Type& data) {
  int idx = -1;

  for (U32 itr = 0; itr < m_size; ++itr) {
    if (data == m_base[itr]) {
      idx = itr;
      break;
    }
  }

  return idx;
}

template <typename Type>
void Vector<Type>::Remove(const Type& data) {
  const int idx = FindFirst(data);

  if (idx >= 0) {
    for (U32 itr      = idx + 1; itr < m_size; ++itr) {
      m_base[itr - 1] = m_base[itr];
    }

    --m_size;
  }
}

template <typename Type>
void Vector<Type>::Reserve(U32 maxSize) {
  m_maxSize = maxSize;
  m_base    = m_allocator.get().RawNewArray<Type>(m_maxSize);
}

template <typename Type>
void Vector<Type>::Resize(U32 maxSize) {
  m_maxSize = maxSize;
  m_size    = maxSize;
  m_base    = m_allocator.get().RawNewArray<Type>(m_maxSize);
}

template <typename Type>
bool Vector<Type>::IsEmpty() const {
  return m_size == 0;
}

template <typename Type>
void Vector<Type>::InsertAt(U32 idx, const Type& data) {
  assert(idx >= 0 && idx <= m_size);

  for (U32 itr  = m_size; itr > idx; --itr) {
    m_base[itr] = m_base[itr - 1];
  }

  m_base[idx] = data;
}

template <typename Type>
Type* Vector<Type>::Data() {
  return m_base.get();
}

template <typename Type>
const Type* Vector<Type>::Data() const {
  return m_base.get();
}

template <typename Type>
Type& Vector<Type>::operator[](const U32 idx) {
  assert(idx < m_size);
  return m_base[idx];
}

template <typename Type>
Type& Vector<Type>::operator[](const U32 idx) const {
  assert(idx < m_size);
  return m_base[idx];
}

template <typename Type>
template <class InputIt>
void Vector<Type>::Assign(InputIt first, InputIt last) {
  U32 count           = 0;
  for (auto itr       = first; itr != last; ++itr) {
    operator[](count) = *itr;
    ++count;
  }
}

template <typename Type>
typename Vector<Type>::Iterator Vector<Type>::Begin() const {
  return Iterator(this, 0);
}

template <typename Type>
typename Vector<Type>::Iterator Vector<Type>::End() const {
  return Iterator(this, m_size);
}

template <typename Type>
typename Vector<Type>::Iterator Vector<Type>::begin() const {
  return Begin();
}

template <typename Type>
typename Vector<Type>::Iterator Vector<Type>::end() const {
  return End();
}
} // namespace Containers
} // namespace Azura
