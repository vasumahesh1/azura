#pragma once

// for memcpy
#include <cstring>
#include <cassert>

#include "Types.h"
#include "Memory/Allocator.h"

namespace Azura {
namespace Containers {
template <typename Type, U32 MaxSize>
class Array {
public:
  // TODO: Enable When NewDeleteAllocator is ready
  //explicit Array(U32 maxSize);
  explicit Array(Memory::Allocator& alloc);

  ~Array();

  // Copy Ctor
  Array(const Array& other);

  // Move Ctor
  Array(Array&& other) noexcept;

  Array& operator=(const Array& other);
  Array& operator=(Array&& other) noexcept;

  /**
  * \brief Appends data to the end of the array
  * \param data Data to push
  */
  void PushBack(const Type& data);

  /**
  * \brief Removes the last element in the array array
  * \return Element that was popped
  */
  Type Pop();

  /**
  * \brief Searches for the data in the Array
  * \param data Data to search for
  * \return Index if Found, else -1
  */
  int FindFirst(const Type& data);

  /**
  * \brief Searches for the given data in the array and removes it
  * \param data Data to Search for and Remove
  */
  void Remove(const Type& data);

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

  Type& operator[](U32 idx);
  Type& operator[](U32 idx) const;
  Type* Data();

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

    Iterator(const Array* ptr, int index)
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
    const Array* mPtr{nullptr};
    int mIndex{-1};
  };

  /**
  * \brief Returns an Iterator pointing to the beginning of the array. This is similar to begin() of a std::array.
  * \return Iterator
  */
  Iterator Begin() const;

  /**
  * \brief Returns an Iterator pointing to the end of the array. This is similar to end() of a std::array.
  * \return Iterator
  */
  Iterator End() const;

#ifdef BUILD_UNIT_TEST
    Type* GetBackPtr() const { return mBack; };
    Type* GetBasePtr() const { return m_base; };
#endif

private:
  U32 m_size{0};
  U32 m_maxSize{MaxSize};
  Memory::Allocator& m_allocator;
  Memory::UniqueArrayPtr<Type> m_base{nullptr};
};

// TODO: Enable When NewDeleteAllocator is ready
//template <typename Type>
//Array<Type, MaxSize>::Array(const U32 maxSize) : m_maxSize(maxSize), m_allocator(alloc), m_base(m_allocator->NewObjects<Type>(maxSize)) {}

template <typename Type, U32 MaxSize>
Array<Type, MaxSize>::Array(Memory::Allocator& alloc)
  : m_allocator(alloc),
    m_base(m_allocator.RawNewArray<Type>(MaxSize)) {
}

template <typename Type, U32 MaxSize>
Array<Type, MaxSize>::~Array() = default;

// TODO: error with non trivial types need something similar to typename std::enable_if<!std::is_fundamental<Type>::value>::type
template <typename Type, U32 MaxSize>
Array<Type, MaxSize>::Array(const Array& other)
  : m_size(other.m_size),
    m_maxSize(other.m_maxSize),
    m_allocator(other.m_allocator) {

  // Allocate Memory
  m_base = m_allocator.RawNewArray<Type>(m_maxSize);

  // Copy over Contents
  std::memcpy(m_base.get(), other.m_base.get(), m_maxSize * sizeof(Type));
}

template <typename Type, U32 MaxSize>
Array<Type, MaxSize>::Array(Array&& other) noexcept
  : m_size(std::move(other.m_size)),
    m_maxSize(std::move(other.m_maxSize)),
    m_allocator(other.m_allocator),
    m_base(std::move(other.m_base)) {
}

// TODO: error with non trivial types need something similar to typename std::enable_if<!std::is_fundamental<Type>::value>::type
template <typename Type, U32 MaxSize>
Array<Type, MaxSize>& Array<Type, MaxSize>::operator=(const Array& other) {
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

template <typename Type, U32 MaxSize>
Array<Type, MaxSize>& Array<Type, MaxSize>::operator=(Array&& other) noexcept {
  if (this == &other) {
    return *this;
  }

  m_size      = std::move(other.m_size);
  m_maxSize   = std::move(other.m_maxSize);
  m_allocator = other.m_allocator;
  m_base      = std::move(other.m_base);

  return *this;
}

template <typename Type, U32 MaxSize>
void Array<Type, MaxSize>::PushBack(const Type& data) {
  assert(m_size < m_maxSize);

  m_base[m_size] = data;
  ++m_size;
}

template <typename Type, U32 MaxSize>
Type Array<Type, MaxSize>::Pop() {
  assert(m_size > 0);

  Type data = m_base[m_size - 1];
  --m_size;

  return data;
}

template <typename Type, U32 MaxSize>
int Array<Type, MaxSize>::FindFirst(const Type& data) {

  int idx = -1;

  for (U32 itr = 0; itr < m_size; ++itr) {
    if (data == m_base[itr]) {
      idx = itr;
      break;
    }
  }

  return idx;
}

template <typename Type, U32 MaxSize>
void Array<Type, MaxSize>::Remove(const Type& data) {
  const int idx = FindFirst(data);

  if (idx >= 0) {
    for (U32 itr      = idx + 1; itr < m_size; ++itr) {
      m_base[itr - 1] = m_base[itr];
    }

    --m_size;
  }
}

template <typename Type, U32 MaxSize>
bool Array<Type, MaxSize>::IsEmpty() const {
  return m_size == 0;
}

template <typename Type, U32 MaxSize>
void Array<Type, MaxSize>::InsertAt(U32 idx, const Type& data) {
  assert(idx >= 0 && idx <= m_size);

  for (int itr  = m_size; itr > idx; --itr) {
    m_base[itr] = m_base[itr - 1];
  }

  m_base[idx] = data;
}

template <typename Type, U32 MaxSize>
Type& Array<Type, MaxSize>::operator[](const U32 idx) {
  assert(idx < m_size);
  return m_base[idx];
}

template <typename Type, U32 MaxSize>
Type& Array<Type, MaxSize>::operator[](const U32 idx) const {
  assert(idx < m_size);
  return m_base[idx];
}

template <typename Type, U32 MaxSize>
Type* Array<Type, MaxSize>::Data() {
  return m_base.get();
}

template <typename Type, U32 MaxSize>
typename Array<Type, MaxSize>::Iterator Array<Type, MaxSize>::Begin() const {
  return Iterator(this, 0);
}

template <typename Type, U32 MaxSize>
typename Array<Type, MaxSize>::Iterator Array<Type, MaxSize>::End() const {
  return Iterator(this, m_size);
}
} // namespace Containers
} // namespace Azura
