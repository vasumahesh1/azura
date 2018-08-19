#pragma once

// for memcpy
#include <cstring>
#include <iterator>

#include "Types.h"
#include "Memory/Allocator.h"
#include <cassert>
#include "Array.h"

namespace Azura {
namespace Containers {
template <typename Type>
class Vector {
public:
  Vector() = default;
  explicit Vector(Memory::Allocator& alloc);

  // TODO: Enable When NewDeleteAllocator is ready
  //explicit Vector(UINT maxSize);
  Vector(UINT maxSize, Memory::Allocator& alloc);
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
  void EmplaceBack(Type&& data);

  /**
   * \brief Removes the last element in the vector array
   * \return Element that was popped
   */
  Type Pop();

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

  Type& operator[](U32 idx);
  Type& operator[](U32 idx) const;

  U32 GetSize() const { return m_size; }
  U32 GetMaxSize() const { return m_maxSize; }

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

private:
  U32 m_size{0};
  U32 m_maxSize{0};
  Memory::Allocator& m_allocator;
  Memory::UniqueArrayPtr<Type> m_base{nullptr};
};

// TODO: Enable When NewDeleteAllocator is ready
//template <typename Type>
//Vector<Type>::Vector(const UINT maxSize) : m_maxSize(maxSize), m_allocator(alloc), m_base(m_allocator->NewObjects<Type>(maxSize)) {}

template <typename Type>
Vector<Type>::Vector(Memory::Allocator& alloc)
  : m_allocator(alloc) {
}

template <typename Type>
Vector<Type>::Vector(const UINT maxSize, Memory::Allocator& alloc)
  : m_maxSize(maxSize),
    m_allocator(alloc),
    m_base(m_allocator.RawNewArray<Type>(maxSize)) {
}

template <typename Type>
Vector<Type>::~Vector() = default;

// TODO: error with non trivial types need something similar to typename std::enable_if<!std::is_fundamental<Type>::value>::type
template <typename Type>
Vector<Type>::Vector(const Vector& other)
  : m_size(other.m_size),
    m_maxSize(other.m_maxSize),
    m_allocator(other.m_allocator) {

  // Allocate Memory
  m_base = m_allocator.RawNewArray<Type>(m_maxSize);

  // Copy over Contents
  std::memcpy(m_base.get(), other.m_base.get(), m_maxSize * sizeof(Type));
}

template <typename Type>
Vector<Type>::Vector(Vector&& other) noexcept
  : m_size(std::move(other.m_size)),
    m_maxSize(std::move(other.m_maxSize)),
    m_allocator(other.m_allocator),
    m_base(std::move(other.m_base)) {
  other.m_allocator = nullptr;
  other.m_base      = nullptr;
}

// TODO: error with non trivial types need something similar to typename std::enable_if<!std::is_fundamental<Type>::value>::type
template <typename Type>
Vector<Type>& Vector<Type>::operator=(const Vector& other) {
  if (this == &other) {
    return *this;
  }

  m_size      = other.m_size;
  m_maxSize   = other.m_maxSize;
  m_allocator = other.m_allocator;

  // Allocate Memory
  m_base = m_allocator.RawNewArray<Type>(m_maxSize);

  // Copy over Contents
  std::memcpy(m_base.get(), other.m_base.get(), m_maxSize * sizeof(Type));

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

  return *this;
}

template <typename Type>
void Vector<Type>::PushBack(const Type& data) {
  assert(m_size < m_maxSize);

  m_base[m_size] = data;
  ++m_size;
}

template <typename Type>
void Vector<Type>::EmplaceBack(Type&& data) {
  assert(m_size < m_maxSize);

  m_base[m_size] = std::move(data);
  ++m_size;
}

template <typename Type>
Type Vector<Type>::Pop() {
  assert(m_size > 0);

  // TODO(vasumahesh1): should move here?
  Type data = m_base[m_size - 1];
  --m_size;

  return data;
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
  m_base    = m_allocator.RawNewArray<Type>(m_maxSize);
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
typename Vector<Type>::Iterator Vector<Type>::Begin() const {
  return Iterator(this, 0);
}

template <typename Type>
typename Vector<Type>::Iterator Vector<Type>::End() const {
  return Iterator(this, m_size);
}
} // namespace Containers
} // namespace Azura
