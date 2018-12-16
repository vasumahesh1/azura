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

/**
 * \brief      A struct defining the bounds of a container
 *
 * \ingroup    Containers
 *
 * \struct
 *
 * \details    This struct holds two important sizes for containers.
 * A current size and a reserved size.
 * 
 * There are two ways to initialize this extent.
 * 
 * ```cpp
 * auto extent1 = ContainerExtent{0, 5}; // Size = 0; ReservedSize = 5;
 * auto extent2 = ContainerExtent{5}; // Size = 5; ReservedSize = 5;
 * ``` 
 */
struct ContainerExtent {
  U32 m_size;
  U32 m_reserveSize;

  /**
   * \brief      Constructs a ContainerExtent with the same size and reserved size.
   *
   * \ingroup    Containers
   *
   * \param[in]  size  The container size
   */
  explicit ContainerExtent(const U32 size)
    : m_size(size),
      m_reserveSize(size) {
  }

  /**
   * \brief      Constructs a ContainerExtent with the specified size and reservataion size.
   *
   * \ingroup    Containers
   *
   * \param[in]  size         The size
   * \param[in]  reserveSize  The reserve size
   */
  ContainerExtent(const U32 size, const U32 reserveSize)
    : m_size(size),
      m_reserveSize(reserveSize) {
  }
};

/**
 * \brief      Class for customized vector.
 *
 * \ingroup    Containers
 *
 * \details    This class is used to construct a customized Azura vector.
 * Azura vectors take advantage of a custom allocator. The custom allocators allows
 * them to be created on the Stack or the Heap.
 * 
 * They also don't "allocate" new memory.
 * Allocation is handled by the supplied Azura::Memory::Allocator class.
 * 
 * Azura Vectors are also "reserve" first.
 * This basically means that the vector reserves a chunk first, instead of reserving and initializing like std::vector.
 * This is probably the main difference between the two. Here is an example:
 * 
 * ```cpp
 * Containers::Vector<int> arr(5, allocator);
 * // above code allocates a memory upto 5 ints. But arr[0 to 4] doesn't exist.
 * // arr.GetSize() will be 0
 * // arr.GetMaxSize() will be 5
 * 
 * std::vector<int> arr(5);
 * // above code allocates and initializes a memory upto 5 ints.
 * // arr.size() will be 5
 * // arr.capacity() will be 5
 * ```
 * 
 * Both vectors tend to grow. But growing vectors are bad so watch out.
 * Pre-allocate as much as you can beforehand.
 * 
 * The API for Azura Vector is similar but Pascal Case'd mostly.
 * Example:
 * 
 * ```cpp
 * Containers::Vector<int> arr(5, allocator);
 * arr.PushBack(0); // similar to push_back
 * arr.Begin();
 * arr.End();
 * ```
 * Pascal case was selected because Azura keeps the API similar across all C++ code.
 * And most of the Game Engine code that was initially built followed this scheme. 
 * 
 * \tparam     Type  Datatype of the vector
 */
template <typename Type>
class Vector {
public:

  /**
   * \brief      Constructs a 0 sized vector with an allocator
   *
   * \details    An empty vector doesn't reserve any space on the allocator.
   * It is required to reserve the vector before you use it.
   * 
   * Example:
   * ```cpp
   * Containers::Vector<int> arr(allocator);
   * // The vector is still of size 0. The capacity is 0.
   * 
   * // ...
   * // Later in code
   * // ...
   * 
   * arr.Resize(5);
   * // This allocates a 5 sized int vector
   * // The vector is of size 5. The capacity is 5.
   * 
   * // ---- OR ----
   * 
   * arr.Reserve(5);
   * // This allocates a 5 sized int vector
   * // The vector is still of size 0. The capacity is 5.
   * 
   * ```
   * \param      alloc    The allocator
   */
  explicit Vector(Memory::Allocator& alloc);

  /**
   * \brief      Constructs a vector accepting reserved size and allocator
   *
   * \details    This constructor **reserves** the specified size in the vector.
   * The vector is of still size 0.
   * 
   * Example:
   * ```cpp
   * Containers::Vector<int> arr(5, allocator);
   * 
   * // This allocates a 5 sized int vector
   * // The vector is still of size 0. The capacity is 5.
   * ```
   *
   * \param[in]  maxSize  The maximum size
   * \param      alloc    The allocator
   */
  Vector(UINT maxSize, Memory::Allocator& alloc);

  /**
   * \brief      Construct a vector using size and max size.
   *
   * \details    This constructor **reserves and constructs** the specified sizes in the vector.
   * 
   * Example:
   * ```cpp
   * Containers::Vector<int> arr(2, 5, allocator);
   * 
   * // This allocates a 5 sized int vector
   * // The vector is of size 2. The capacity is 5.
   * ```
   *
   * \param[in]  currentSize  The current size
   * \param[in]  maxSize      The maximum size
   * \param      alloc        The allocator
   */
  Vector(UINT currentSize, UINT maxSize, Memory::Allocator& alloc);

  /**
   * \brief      Construct a vector using an initializer list
   *
   * \details
   * 
   * Example:
   * ```cpp
   * Containers::Vector<int> arr({1, 2, 3, 4, 5}, allocator);
   * 
   * // This allocates a 5 sized int vector with that values
   * // The vector is of size 5. The capacity is 5.
   * ```
   *
   * \param[in]  list   The list
   * \param      alloc  The allocator
   */
  Vector(const std::initializer_list<Type>& list, Memory::Allocator& alloc);

  /**
   * \brief      Constructs the vector using the extent.
   * 
   * \details    Also, you can provide initialization arguments along with the extent.
   * 
   * Example:
   * ```cpp
   * Containers::Vector<int> arr(ContainerExtent{5}, allocator, 6);
   * 
   * // This allocates a 5 sized int vector with the value 6.
   * // The vector is of size 5. The capacity is 5.
   * ```
   *
   * \param[in]  extent     The extent
   * \param      alloc      The allocator
   * \param[in]  args       Initialization arguments
   *
   * \tparam     Args       Arguments
   */
  template <typename... Args>
  Vector(ContainerExtent extent, Memory::Allocator& alloc, Args&&... args);

  /**
   * \brief      Destroys the vector and frees the memory in the allocator.
   */
  ~Vector();

  Vector(const Vector& other);
  Vector(const Vector & other, Memory::Allocator & alloc);
  Vector(Vector&& other) noexcept;
  Vector& operator=(const Vector& other);
  Vector& operator=(Vector&& other) noexcept;

  /**
   * \brief      Appends data to the end of the vector
   * 
   * \details    The vector also doubles its size incase it fills up. Recommended to pre-allocate as much as you can.
   * 
   * Example:
   * ```cpp
   * Containers::Vector<int> arr(5, allocator);
   * // The vector is of size 0. The capacity is 5.
   * 
   * arr.PushBack(1);
   * // The vector is of size 1. The capacity is 5.
   * 
   * arr.PushBack(2);
   * // The vector is of size 2. The capacity is 5.
   * 
   * arr.PushBack(3);
   * // The vector is of size 3. The capacity is 5.
   * 
   * arr.PushBack(4);
   * // The vector is of size 4. The capacity is 5.
   * ```
   * 
   * \param data Data to push
   */
  void PushBack(const Type& data);

  /**
   * \copydoc Vector::PushBack(const Type&)
   */
  void PushBack(Type&& data);

  /**
   * \brief      Appends data to the end of the vector by emplacing (like regular vector)
   * 
   * \details    The vector also doubles its size incase it fills up. Recommended to pre-allocate as much as you can.
   * 
   * The arguments are the initialization parameters for the Type.
   * 
   * Example:
   * ```cpp
   * MyClass {
   * public:
   *   MyClass(int a) : data(a) {}
   *   
   * private:
   *   int data;
   * };
   * 
   * Containers::Vector<MyClass> arr(5, allocator);
   * // The vector is of size 0. The capacity is 5.
   * 
   * arr.EmplaceBack(1); // Creates a MyClass{1} at the memory location
   * // The vector is of size 1. The capacity is 5.
   * 
   * arr.EmplaceBack(2); // Creates a MyClass{2} at the memory location
   * // The vector is of size 2. The capacity is 5.
   * ```
   * 
   * \param args Arguments to push
   * 
   * \tparam     Args       Arguments
   */
  template <typename... Args>
  void EmplaceBack(Args ... args);

  /**
   * \brief Removes the last element in the vector array
   */
  void PopBack();

  /**
   * \brief Searches for the data in the vector.
   * 
   * \param data Data to search for
   * 
   * \return Index if Found, else -1
   */
  int FindFirst(const Type& data);

  /**
   * \brief Searches for the given data in the vector and removes it.
   *
   * \param data Data to Search for and Remove
   */
  void Remove(const Type& data);

  /**
   * \brief      Reserves a contiguous block for the vector.
   * 
   * \details    Use this when you didn't provide an initial size for the vector.
   * The growth of the vector is not controlled by this.
   * Using Reserve **after** storing the data in the vector will lead an undefined behavior.
   * Use this only once.
   * 
   * Example:
   * ```cpp
   * Containers::Vector<int> arr(allocator);
   * // The vector is still of size 0. The capacity is 0.
   * 
   * // ...
   * // Later in code
   * // ...
   *
   * arr.Reserve(5);
   * // This allocates a 5 sized int vector
   * // The vector is still of size 0. The capacity is 5.
   *
   * ```
   *
   * \param requiredSize Required Size
   */
  void Reserve(U32 requiredSize);

  /**
   * \brief      Reserves a contiguous block for the vector and sets the size to the value specified.
   * 
   * \details    Also, sets the size to the specified value.
   * Use this when you didn't provide an initial size for the vector.
   * The growth of the vector is not controlled by this.
   * Using Resize **after** storing the data in the vector will lead an undefined behavior.
   * Use this only once.
   * 
   * Example:
   * ```cpp
   * Containers::Vector<int> arr(allocator);
   * // The vector is still of size 0. The capacity is 0.
   * 
   * // ...
   * // Later in code
   * // ...
   *
   * arr.Resize(5);
   * // This allocates a 5 sized int vector
   * // The vector is now of size 5. The capacity is 5.
   *
   * ```
   *
   * \param requiredSize Required Size
   */
  void Resize(U32 requiredSize);

  /**
   * \brief Checks if the container is empty
   *
   * \return `true` if empty
   */
  bool IsEmpty() const;

  /**
   * \brief Inserts the supplied data at the index
   *
   * \todo Better Documentation
   *
   * \param idx Target Index
   * \param data Data to insert
   */
  void InsertAt(U32 idx, const Type& data);

  /**
   * \brief Gets the Data pointer
   * 
   * \details
   * 
   * Example:
   * ```cpp
   * Containers::Vector<int> arr(5, allocator);
   * // The vector is still of size 0. The capacity is 5.
   * 
   * arr.Data() // is of type int*
   * ```
   * 
   */
  Type* Data();

  /**
   * \copybrief Vector::Data()
   *
   * \copydetails Vector::Data()
   * Useful as a constant pointer when compared to its other overload.
   */
  const Type* Data() const;

  /**
   * \brief Empties the vector.
   * 
   * \details But, it doesn't deallocate anything.
   * It is meant to reuse the allocated memory.
   * 
   * Example:
   * ```cpp
   * Containers::Vector<int> arr(5, allocator);
   * // The vector is of size 0. The capacity is 5.
   * 
   * arr.PushBack(1);
   * // The vector is of size 1. The capacity is 5.
   * 
   * arr.PushBack(2);
   * // The vector is of size 2. The capacity is 5.
   * 
   * arr.PushBack(3);
   * // The vector is of size 3. The capacity is 5.
   * 
   * arr.PushBack(4);
   * // The vector is of size 4. The capacity is 5.
   * 
   * arr.Reset(); // Or Clear();
   * // The vector is of size 0. The capacity is 5.
   * ```
   * 
   */
  void Reset();

  /**
   * \copybrief void Vector::Reset()
   *
   * \copydetails void Vector::Reset()
   */
  void Clear();

  Type& operator[](U32 idx);
  Type& operator[](U32 idx) const;

  Type& Last();
  Type& Last() const;

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
  void GrowIfNeeded();

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
Vector<Type>::Vector(UINT currentSize, UINT maxSize, Memory::Allocator& alloc)
    :
    m_size(currentSize),
    m_maxSize(maxSize),
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
Vector<Type>::Vector(const Vector& other, Memory::Allocator& alloc)
  : m_size(other.m_size),
  m_maxSize(other.m_maxSize),
  m_allocator(alloc) {
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
void Vector<Type>::GrowIfNeeded() {
  if (m_size < m_maxSize)
  {
    return;
  }

  if (m_maxSize == 0)
  {
#ifdef BUILD_DEBUG
    printf("=== Debug Warning: Vector was not initialized with any size. Set a size using the ctor or Reserve() or Resize(). ===\n");
#endif
    Reserve(1);
    return;
  }

#ifdef BUILD_DEBUG
  printf("=== Debug Warning: Vector Growing in Size. This is underperformant -- consider setting the initial size. ===\n");
#endif

  m_maxSize = 2 * m_maxSize;

  // Grow Vector
  auto newDataHandle = m_allocator.get().RawNewArray<Type>(m_maxSize);

  if constexpr (std::is_trivially_copyable_v<Type>) {
    // Copy over Contents
    std::memmove(newDataHandle.get(), m_base.get(), m_size * sizeof(Type));
  }
  else {
    // Manually Move Construct each item into new space
    for (U32 idx = 0; idx < m_size; ++idx) {
      new(&newDataHandle[idx]) Type(std::move(m_base[idx]));

      // TODO(vasumahesh1): MSVC gives a NoDiscard warning here. Not sure why, but due to C++17.
      UNUSED(m_base[idx].~Type());
    }
  }

  m_base = std::move(newDataHandle);
}

template <typename Type>
void Vector<Type>::PushBack(const Type& data) {
  GrowIfNeeded();

  new(&m_base[m_size]) Type(data);
  ++m_size;
}

template <typename Type>
void Vector<Type>::PushBack(Type&& data) {
  GrowIfNeeded();

  new(&m_base[m_size]) Type(std::move(data));
  ++m_size;
}

template <typename Type>
template <typename... Args>
void Vector<Type>::EmplaceBack(Args ... args) {
  GrowIfNeeded();

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
void Vector<Type>::Reserve(U32 requiredSize) {
  m_maxSize = requiredSize;
  m_base    = m_allocator.get().RawNewArray<Type>(m_maxSize);
}

template <typename Type>
void Vector<Type>::Resize(U32 requiredSize) {
  m_maxSize = requiredSize;
  m_size    = requiredSize;
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
void Vector<Type>::Reset() {
  m_size = 0;
}

template <typename Type>
void Vector<Type>::Clear() {
  m_size = 0;
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
Type& Vector<Type>::Last() {
  return m_base[m_size - 1];
}

template <typename Type>
Type& Vector<Type>::Last() const {
  return m_base[m_size - 1];
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
