#pragma once

// for memcpy
#include <cstring>
#include <cassert>

#include "Types.h"
#include "MemoryAllocators/Allocator.h"

namespace Azura {
template <typename Type, UINT MaxSize> class Array {
public:
  // TODO: Enable When NewDeleteAllocator is ready
  //explicit Array(UINT maxSize);
  explicit Array(Allocator *alloc);

  ~Array();

  // Copy Ctor
  Array(const Array &other);

  // Move Ctor
  Array(Array &&other) noexcept;

  Array &operator=(const Array &other);
  Array &operator=(Array &&other) noexcept;

  /**
  * \brief Appends data to the end of the array
  * \param data Data to push
  */
  void Push(const Type &data);

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
  int Find(const Type &data);

  /**
  * \brief Searches for the given data in the array and removes it
  * \param data Data to Search for and Remove
  */
  void Remove(const Type &data);

  /**
  * \brief Reserves a max contiguous block for the array
  * Use only when you didn't supply a maxSize in the constructor.
  * \param maxSize Maxium Possible Size
  */
  void Reserve(UINT maxSize);

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
  void InsertAt(UINT idx, const Type &data);

  Type &operator[](UINT idx);
  Type &operator[](UINT idx) const;

  UINT GetSize() const { return mSize; }
  UINT GetMaxSize() const { return mMaxSize; }

  class Iterator : public std::iterator<std::random_access_iterator_tag, Type> {
  public:
    Iterator() = default;
    ~Iterator() = default;
    Iterator(const Iterator &other) = default;
    Iterator &operator=(const Iterator &other) = default;

    Iterator(const Array *ptr, int index)
      : mPtr(ptr),
        mIndex(index) {
    }

    Iterator(Iterator &&other) noexcept = default;
    Iterator &operator=(Iterator &&other) noexcept = default;

    // Pre Increment
    Iterator &operator++() {
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
    Iterator &operator--() {
      --mIndex;
      return *this;
    }

    // Post Decrement
    Iterator operator--(int) {
      Iterator copy(*this);
      operator--();
      return copy;
    }

    bool operator==(const Iterator &rhs) {
      return mPtr == rhs.mPtr && mIndex == rhs.mIndex;
    }

    bool operator!=(const Iterator &rhs) {
      return !(*this == rhs);
    }

    bool operator<(const Iterator &rhs) {
      assert(mPtr == rhs.mPtr);
      return mIndex < rhs.mIndex;
    }

    bool operator<=(const Iterator &rhs) {
      assert(mPtr == rhs.mPtr);
      return mIndex <= rhs.mIndex;
    }

    bool operator>(const Iterator &rhs) {
      assert(mPtr == rhs.mPtr);
      return mIndex > rhs.mIndex;
    }

    bool operator>=(const Iterator &rhs) {
      assert(mPtr == rhs.mPtr);
      return mIndex >= rhs.mIndex;
    }

    Iterator operator+(const int &idx) {
      return Iterator(mPtr, mIndex + idx);
    }

    Iterator &operator+=(const int &idx) {
      mIndex += idx;
      return *this;
    }

    Iterator operator-(const int &idx) {
      assert(mIndex - idx > 0);
      return Iterator(mPtr, mIndex - idx);
    }

    Iterator &operator-=(const int &idx) {
      assert(mIndex - idx > 0);
      mIndex -= idx;
      return *this;
    }

    int operator-(const Iterator &rhs) {
      assert(mPtr == rhs.mPtr);
      return mIndex - rhs.mIndex;
    }

    friend int operator-(const Iterator &lhs, const Iterator &rhs) {
      Iterator copy(lhs);
      return copy - rhs;
    }

    // Element Accessors

    Type &operator*() {
      return mPtr->operator[](mIndex);
    }

    Type *operator->() {
      return &mPtr->operator[](mIndex);
    }

    Type &operator[](const int &idx) {
      return mPtr->operator[](mIndex + idx);
    }

  private:
    const Array *mPtr{nullptr};
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

#ifdef TOOLS_TEST
    Type* GetBackPtr() const { return mBack; };
    Type* GetBasePtr() const { return mBase; };
#endif

private:
  UINT mSize{0};
  UINT mMaxSize{MaxSize};
  Allocator *mAlloc{nullptr};
  Type *mBase{nullptr};
  Type *mBack{nullptr};
};

// TODO: Enable When NewDeleteAllocator is ready
//template <typename Type>
//Array<Type, MaxSize>::Array(const UINT maxSize) : mMaxSize(maxSize), mAlloc(alloc), mBase(mAlloc->NewObjects<Type>(maxSize)) {}

template <typename Type, UINT MaxSize> Array<Type, MaxSize>::Array(
  Allocator *alloc)
  : mAlloc(alloc),
    mBase(mAlloc->NewObjects<Type>(MaxSize, false)),
    mBack(mBase) {
}

template <typename Type, UINT MaxSize> Array<Type, MaxSize>::~Array() {
  if (mAlloc) {
    mAlloc->DeleteObjects(mBase, mMaxSize);
  }
}

// TODO: error with non trivial types need something similar to typename std::enable_if<!std::is_fundamental<Type>::value>::type
template <typename Type, UINT MaxSize> Array<Type, MaxSize>::Array(
  const Array &other)
  : mSize(other.mSize),
    mMaxSize(other.mMaxSize),
    mAlloc(other.mAlloc),
    mBack(other.mBack) {

  // Allocate Memory
  mBase = mAlloc->NewObjects<Type>(mMaxSize, false);

  // Copy over Contents
  std::memcpy(mBase, other.mBase, mMaxSize * sizeof(Type));
}

template <typename Type, UINT MaxSize> Array<Type, MaxSize>::Array(
  Array &&other) noexcept
  : mSize(other.mSize),
    mMaxSize(other.mMaxSize),
    mAlloc(other.mAlloc),
    mBase(other.mBase),
    mBack(other.mBack) {
  other.mAlloc = nullptr;
  other.mBase = nullptr;
  other.mBack = nullptr;
}

// TODO: error with non trivial types need something similar to typename std::enable_if<!std::is_fundamental<Type>::value>::type
template <typename Type, UINT MaxSize> Array<Type, MaxSize> &Array<
  Type, MaxSize>::operator=(const Array &other) {
  if (this == &other) {
    return *this;
  }

  mSize = other.mSize;
  mMaxSize = other.mMaxSize;
  mAlloc = other.mAlloc;
  mBack = other.mBack;

  // Allocate Memory
  mBase = mAlloc->NewObjects<Type>(mMaxSize, false);

  // Copy over Contents
  std::memcpy(mBase, other.mBase, mMaxSize * sizeof(Type));

  return *this;
}

template <typename Type, UINT MaxSize> Array<Type, MaxSize> &Array<
  Type, MaxSize>::operator=(Array &&other) noexcept {
  if (this == &other) {
    return *this;
  }

  mSize = other.mSize;
  mMaxSize = other.mMaxSize;
  mAlloc = other.mAlloc;
  mBase = other.mBase;
  mBack = other.mBack;

  other.mAlloc = nullptr;
  other.mBase = nullptr;

  return *this;
}

template <typename Type, UINT MaxSize> void Array<Type, MaxSize>::Push(
  const Type &data) {
  assert(mSize < mMaxSize);

  *mBack = data;
  ++mBack;
  ++mSize;
}

template <typename Type, UINT MaxSize> Type Array<Type, MaxSize>::Pop() {
  assert(mSize > 0);

  Type data = *(mBack - 1);
  --mSize;
  --mBack;

  return data;
}

template <typename Type, UINT MaxSize> int Array<Type, MaxSize>::Find(
  const Type &data) {
  Type *start = mBase;

  int idx = -1;

  for (auto itr = 0; itr < mSize; ++itr) {
    if (data == *start) {
      idx = itr;
      break;
    }

    ++start;
  }

  return idx;
}

template <typename Type, UINT MaxSize> void Array<Type, MaxSize>::Remove(
  const Type &data) {
  Type *start = mBase;
  const int idx = Find(data);

  if (idx >= 0) {
    Type *ptr = start + (idx + 1);

    for (auto itr = idx + 1; itr < mSize; ++itr) {
      *(ptr - 1) = *ptr;
      ++ptr;
    }

    --mBack;
    --mSize;
  }
}

template <typename Type, UINT MaxSize> void Array<Type, MaxSize>::Reserve(
  UINT maxSize) {
  mMaxSize = maxSize;
  mBase = mAlloc->NewObjects<Type>(maxSize, false);
}

template <typename Type, UINT MaxSize> bool Array<Type, MaxSize>::
IsEmpty() const {
  return mSize == 0;
}

template <typename Type, UINT MaxSize> void Array<Type, MaxSize>::InsertAt(
  UINT idx, const Type &data) {
  assert(idx >= 0 && idx <= mSize);

  Type *ptr = mBack;

  for (int i = mSize; i > idx; i--) {
    *ptr = *(ptr - 1);
    --ptr;
  }

  *(mBase + idx) = data;
}

template <typename Type, UINT MaxSize> Type &Array<Type, MaxSize>::operator[](
  const UINT idx) {
  assert(idx < mSize);
  return *(mBase + idx);
}

template <typename Type, UINT MaxSize> Type &Array<Type, MaxSize>::operator[](
  const UINT idx) const {
  assert(idx < mSize);
  return *(mBase + idx);
}

template <typename Type, UINT MaxSize> typename Array<Type, MaxSize>::Iterator
Array<Type, MaxSize>::Begin() const {
  return Iterator(this, 0);
}

template <typename Type, UINT MaxSize> typename Array<Type, MaxSize>::Iterator
Array<Type, MaxSize>::End() const {
  return Iterator(this, mSize);
}
} // namespace AZ
