# group `Containers` {#group___containers}

This module consists of customized containers that are useful for certain applications.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public inline  explicit `[`ContainerExtent`](#group___containers_1gabdea46cc128f48a9fc317fd5ca797ee8)`(const U32 size)`            | Constructs a [ContainerExtent](#struct_azura_1_1_containers_1_1_container_extent) with the same size and reserved size.
`public inline  `[`ContainerExtent`](#group___containers_1ga086b568dd8be510400cd02bf146c6141)`(const U32 size,const U32 reserveSize)`            | Constructs a [ContainerExtent](#struct_azura_1_1_containers_1_1_container_extent) with the specified size and reservataion size.
`class `[`Azura::Containers::Vector`](Docs/Module - Containers.md#class_azura_1_1_containers_1_1_vector) | Class for customized vector.

## Members

#### `public inline  explicit `[`ContainerExtent`](#group___containers_1gabdea46cc128f48a9fc317fd5ca797ee8)`(const U32 size)` {#group___containers_1gabdea46cc128f48a9fc317fd5ca797ee8}

Constructs a [ContainerExtent](#struct_azura_1_1_containers_1_1_container_extent) with the same size and reserved size.

#### Parameters
* `size` The container size

#### `public inline  `[`ContainerExtent`](#group___containers_1ga086b568dd8be510400cd02bf146c6141)`(const U32 size,const U32 reserveSize)` {#group___containers_1ga086b568dd8be510400cd02bf146c6141}

Constructs a [ContainerExtent](#struct_azura_1_1_containers_1_1_container_extent) with the specified size and reservataion size.

#### Parameters
* `size` The size 

* `reserveSize` The reserve size

# class `Azura::Containers::Vector` {#class_azura_1_1_containers_1_1_vector}

Class for customized vector.

This class is used to construct a customized Azura vector. Azura vectors take advantage of a custom allocator. The custom allocators allows them to be created on the Stack or the Heap.

They also don't "allocate" new memory. Allocation is handled by the supplied [Azura::Memory::Allocator](#class_azura_1_1_memory_1_1_allocator) class.

Azura Vectors are also "reserve" first. This basically means that the vector reserves a chunk first, instead of reserving and initializing like std::vector. This is probably the main difference between the two. Here is an example:

```cpp
Containers::Vector<int> arr(5, allocator);
// above code allocates a memory upto 5 ints. But arr[0 to 4] doesn't exist.
// arr.GetSize() will be 0
// arr.GetMaxSize() will be 5

std::vector<int> arr(5);
// above code allocates and initializes a memory upto 5 ints.
// arr.size() will be 5
// arr.capacity() will be 5
```

Both vectors tend to grow. But growing vectors are bad so watch out. Pre-allocate as much as you can beforehand.

The API for Azura [Vector](#class_azura_1_1_containers_1_1_vector) is similar but Pascal Case'd mostly. Example:

```cpp
Containers::Vector<int> arr(5, allocator);
arr.PushBack(0); // similar to push_back
arr.Begin();
arr.End();
```
 Pascal case was selected because Azura keeps the API similar across all C++ code. And most of the Game Engine code that was initially built followed this scheme.

#### Parameters
* `Type` Datatype of the vector

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  explicit `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a6f3debb85587cf01eef9bddcb14c58fb)`(`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc)` | Constructs a 0 sized vector with an allocator.
`public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a57b086de3f62679d082a086300138636)`(UINT maxSize,`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc)` | Constructs a vector accepting reserved size and allocator.
`public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1af816fc5ac5a9e5060f6c88cb4e8beaa3)`(UINT currentSize,UINT maxSize,`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc)` | Construct a vector using size and max size.
`public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a791c8721cacac5dd785a45a98e45d4a8)`(const std::initializer_list< Type > & list,`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc)` | Construct a vector using an initializer list.
`public template<>`  <br/>` `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a11574d6d332a04eccebcc4f0255275b4)`(`[`ContainerExtent`](#struct_azura_1_1_containers_1_1_container_extent)` extent,`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc,Args &&... args)` | Constructs the vector using the extent.
`public  `[`~Vector`](#class_azura_1_1_containers_1_1_vector_1a81495c5eec68f60da33123073cdde942)`()` | Destroys the vector and frees the memory in the allocator.
`public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a3b25f7177d538d7bb646ceb5fa46f30c)`(const `[`Vector`](#class_azura_1_1_containers_1_1_vector)` & other)` | 
`public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1afa47d6fd1b56460466a5b15a1030c439)`(const `[`Vector`](#class_azura_1_1_containers_1_1_vector)` & other,`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc)` | 
`public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a2552d6447e1aeccdd359a50f9c52d016)`(`[`Vector`](#class_azura_1_1_containers_1_1_vector)` && other) noexcept` | 
`public `[`Vector`](#class_azura_1_1_containers_1_1_vector)` & `[`operator=`](#class_azura_1_1_containers_1_1_vector_1adbb949f6f25479775bb3af194ce528d5)`(const `[`Vector`](#class_azura_1_1_containers_1_1_vector)` & other)` | 
`public `[`Vector`](#class_azura_1_1_containers_1_1_vector)` & `[`operator=`](#class_azura_1_1_containers_1_1_vector_1aed6c43d8742cd53f938c87ae91e6532b)`(`[`Vector`](#class_azura_1_1_containers_1_1_vector)` && other) noexcept` | 
`public void `[`PushBack`](#class_azura_1_1_containers_1_1_vector_1aadc803b734b7481d9d7f2e9f86891ce4)`(const Type & data)` | Appends data to the end of the vector.
`public void `[`PushBack`](#class_azura_1_1_containers_1_1_vector_1af55839be4b5b355ed020a09201713935)`(Type && data)` | Appends data to the end of the vector.
`public template<>`  <br/>`void `[`EmplaceBack`](#class_azura_1_1_containers_1_1_vector_1aadfac4f55765e41e812160022a9a949f)`(Args ... args)` | Appends data to the end of the vector by emplacing (like regular vector)
`public void `[`PopBack`](#class_azura_1_1_containers_1_1_vector_1ad181b9dc858e1ad59fc64b39f795fb75)`()` | Removes the last element in the vector array.
`public int `[`FindFirst`](#class_azura_1_1_containers_1_1_vector_1a5da3557964b80e7c40a574fbbb7e24a6)`(const Type & data)` | Searches for the data in the vector.
`public void `[`Remove`](#class_azura_1_1_containers_1_1_vector_1a4d191b0d876f03c44c5a58e6380a07ee)`(const Type & data)` | Searches for the given data in the vector and removes it.
`public void `[`Reserve`](#class_azura_1_1_containers_1_1_vector_1af53da95e8dee524b3b12f2208079741f)`(U32 requiredSize)` | Reserves a contiguous block for the vector.
`public void `[`Resize`](#class_azura_1_1_containers_1_1_vector_1a6d651ba60555cf392c766eb77758bd81)`(U32 requiredSize)` | Reserves a contiguous block for the vector and sets the size to the value specified.
`public bool `[`IsEmpty`](#class_azura_1_1_containers_1_1_vector_1a3f955f930eefbb486ff9880421bbc7a4)`() const` | Checks if the container is empty.
`public void `[`InsertAt`](#class_azura_1_1_containers_1_1_vector_1a9f350f9bbc7f5aaec6318369444d2d99)`(U32 idx,const Type & data)` | Inserts the supplied data at the index.
`public Type * `[`Data`](#class_azura_1_1_containers_1_1_vector_1a6e3fd44d2821bb97f170b6e5443ec57e)`()` | Gets the Data pointer.
`public const Type * `[`Data`](#class_azura_1_1_containers_1_1_vector_1a512f49e419efb7f312d0a08ddfc675b7)`() const` | Gets the Data pointer.
`public void `[`Reset`](#class_azura_1_1_containers_1_1_vector_1a9a0fe84cbf357050432a0f851f8a8daa)`()` | Empties the vector.
`public void `[`Clear`](#class_azura_1_1_containers_1_1_vector_1acf0e0039895ef1262e9060406863351e)`()` | [Vector::Reset()](Docs/Module - Containers.md#class_azura_1_1_containers_1_1_vector_1a9a0fe84cbf357050432a0f851f8a8daa)
`public Type & `[`operator[]`](#class_azura_1_1_containers_1_1_vector_1a9375fcc59ae44bfe9960d045d322d897)`(U32 idx)` | 
`public Type & `[`operator[]`](#class_azura_1_1_containers_1_1_vector_1aa88a35925d3013cd8050639e07938d62)`(U32 idx) const` | 
`public Type & `[`Last`](#class_azura_1_1_containers_1_1_vector_1aff0a689dcc4d698e49a40453f4e40d3b)`()` | 
`public Type & `[`Last`](#class_azura_1_1_containers_1_1_vector_1a6c87f384a7a0e3c4a76e53c8607e7f04)`() const` | 
`public inline U32 `[`GetSize`](#class_azura_1_1_containers_1_1_vector_1a109fab9164fcbf717f7dba81e1a8ac47)`() const` | 
`public inline U32 `[`GetMaxSize`](#class_azura_1_1_containers_1_1_vector_1ad51431e69d28f4f8e240c8f64cea17db)`() const` | 
`public template<>`  <br/>`void `[`Assign`](#class_azura_1_1_containers_1_1_vector_1a41f8b0d08472230bbcaf4e9611c9a5b3)`(InputIt first,InputIt last)` | 
`public `[`Iterator`](#class_azura_1_1_containers_1_1_vector_1_1_iterator)` `[`Begin`](#class_azura_1_1_containers_1_1_vector_1a49e7eba07315528d8f75998944b0e7c5)`() const` | Returns an [Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator) pointing to the beginning of the vector. This is similar to [begin()](#class_azura_1_1_containers_1_1_vector_1a324d28755f053b4a83c89982eebeee93) of a std::vector.
`public `[`Iterator`](#class_azura_1_1_containers_1_1_vector_1_1_iterator)` `[`End`](#class_azura_1_1_containers_1_1_vector_1aaf8c1fcfb814e8030e5520b56c094c6a)`() const` | Returns an [Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator) pointing to the end of the vector. This is similar to [end()](#class_azura_1_1_containers_1_1_vector_1a364c485414869aa9d6de832f2af0f46f) of a std::vector.
`public `[`Iterator`](#class_azura_1_1_containers_1_1_vector_1_1_iterator)` `[`begin`](#class_azura_1_1_containers_1_1_vector_1a324d28755f053b4a83c89982eebeee93)`() const` | Returns an [Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator) pointing to the beginning of the vector. This is similar to [begin()](#class_azura_1_1_containers_1_1_vector_1a324d28755f053b4a83c89982eebeee93) of a std::vector.
`public `[`Iterator`](#class_azura_1_1_containers_1_1_vector_1_1_iterator)` `[`end`](#class_azura_1_1_containers_1_1_vector_1a364c485414869aa9d6de832f2af0f46f)`() const` | Returns an [Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator) pointing to the end of the vector. This is similar to [end()](#class_azura_1_1_containers_1_1_vector_1a364c485414869aa9d6de832f2af0f46f) of a std::vector.

## Members

#### `public  explicit `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a6f3debb85587cf01eef9bddcb14c58fb)`(`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc)` {#class_azura_1_1_containers_1_1_vector_1a6f3debb85587cf01eef9bddcb14c58fb}

Constructs a 0 sized vector with an allocator.

An empty vector doesn't reserve any space on the allocator. It is required to reserve the vector before you use it.

Example: 
```cpp
Containers::Vector<int> arr(allocator);
// The vector is still of size 0. The capacity is 0.

// ...
// Later in code
// ...

arr.Resize(5);
// This allocates a 5 sized int vector
// The vector is of size 5. The capacity is 5.

// ---- OR ----

arr.Reserve(5);
// This allocates a 5 sized int vector
// The vector is still of size 0. The capacity is 5.
```

#### Parameters
* `alloc` The allocator

#### `public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a57b086de3f62679d082a086300138636)`(UINT maxSize,`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc)` {#class_azura_1_1_containers_1_1_vector_1a57b086de3f62679d082a086300138636}

Constructs a vector accepting reserved size and allocator.

This constructor **reserves** the specified size in the vector. The vector is of still size 0.

Example: 
```cpp
Containers::Vector<int> arr(5, allocator);

// This allocates a 5 sized int vector
// The vector is still of size 0. The capacity is 5.
```

#### Parameters
* `maxSize` The maximum size 

* `alloc` The allocator

#### `public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1af816fc5ac5a9e5060f6c88cb4e8beaa3)`(UINT currentSize,UINT maxSize,`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc)` {#class_azura_1_1_containers_1_1_vector_1af816fc5ac5a9e5060f6c88cb4e8beaa3}

Construct a vector using size and max size.

This constructor **reserves and constructs** the specified sizes in the vector.

Example: 
```cpp
Containers::Vector<int> arr(2, 5, allocator);

// This allocates a 5 sized int vector
// The vector is of size 2. The capacity is 5.
```

#### Parameters
* `currentSize` The current size 

* `maxSize` The maximum size 

* `alloc` The allocator

#### `public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a791c8721cacac5dd785a45a98e45d4a8)`(const std::initializer_list< Type > & list,`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc)` {#class_azura_1_1_containers_1_1_vector_1a791c8721cacac5dd785a45a98e45d4a8}

Construct a vector using an initializer list.

Example: 
```cpp
Containers::Vector<int> arr({1, 2, 3, 4, 5}, allocator);

// This allocates a 5 sized int vector with that values
// The vector is of size 5. The capacity is 5.
```

#### Parameters
* `list` The list 

* `alloc` The allocator

#### `public template<>`  <br/>` `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a11574d6d332a04eccebcc4f0255275b4)`(`[`ContainerExtent`](#struct_azura_1_1_containers_1_1_container_extent)` extent,`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc,Args &&... args)` {#class_azura_1_1_containers_1_1_vector_1a11574d6d332a04eccebcc4f0255275b4}

Constructs the vector using the extent.

Also, you can provide initialization arguments along with the extent.

Example: 
```cpp
Containers::Vector<int> arr([ContainerExtent](#struct_azura_1_1_containers_1_1_container_extent){5}, allocator, 6);

// This allocates a 5 sized int vector with the value 6.
// The vector is of size 5. The capacity is 5.
```

#### Parameters
* `extent` The extent 

* `alloc` The allocator 

* `args` Initialization arguments

#### Parameters
* `Args` Arguments

#### `public  `[`~Vector`](#class_azura_1_1_containers_1_1_vector_1a81495c5eec68f60da33123073cdde942)`()` {#class_azura_1_1_containers_1_1_vector_1a81495c5eec68f60da33123073cdde942}

Destroys the vector and frees the memory in the allocator.

#### `public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a3b25f7177d538d7bb646ceb5fa46f30c)`(const `[`Vector`](#class_azura_1_1_containers_1_1_vector)` & other)` {#class_azura_1_1_containers_1_1_vector_1a3b25f7177d538d7bb646ceb5fa46f30c}

#### `public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1afa47d6fd1b56460466a5b15a1030c439)`(const `[`Vector`](#class_azura_1_1_containers_1_1_vector)` & other,`[`Memory::Allocator`](#class_azura_1_1_memory_1_1_allocator)` & alloc)` {#class_azura_1_1_containers_1_1_vector_1afa47d6fd1b56460466a5b15a1030c439}

#### `public  `[`Vector`](#class_azura_1_1_containers_1_1_vector_1a2552d6447e1aeccdd359a50f9c52d016)`(`[`Vector`](#class_azura_1_1_containers_1_1_vector)` && other) noexcept` {#class_azura_1_1_containers_1_1_vector_1a2552d6447e1aeccdd359a50f9c52d016}

#### `public `[`Vector`](#class_azura_1_1_containers_1_1_vector)` & `[`operator=`](#class_azura_1_1_containers_1_1_vector_1adbb949f6f25479775bb3af194ce528d5)`(const `[`Vector`](#class_azura_1_1_containers_1_1_vector)` & other)` {#class_azura_1_1_containers_1_1_vector_1adbb949f6f25479775bb3af194ce528d5}

#### `public `[`Vector`](#class_azura_1_1_containers_1_1_vector)` & `[`operator=`](#class_azura_1_1_containers_1_1_vector_1aed6c43d8742cd53f938c87ae91e6532b)`(`[`Vector`](#class_azura_1_1_containers_1_1_vector)` && other) noexcept` {#class_azura_1_1_containers_1_1_vector_1aed6c43d8742cd53f938c87ae91e6532b}

#### `public void `[`PushBack`](#class_azura_1_1_containers_1_1_vector_1aadc803b734b7481d9d7f2e9f86891ce4)`(const Type & data)` {#class_azura_1_1_containers_1_1_vector_1aadc803b734b7481d9d7f2e9f86891ce4}

Appends data to the end of the vector.

The vector also doubles its size incase it fills up. Recommended to pre-allocate as much as you can.

Example: 
```cpp
Containers::Vector<int> arr(5, allocator);
// The vector is of size 0. The capacity is 5.

arr.PushBack(1);
// The vector is of size 1. The capacity is 5.

arr.PushBack(2);
// The vector is of size 2. The capacity is 5.

arr.PushBack(3);
// The vector is of size 3. The capacity is 5.

arr.PushBack(4);
// The vector is of size 4. The capacity is 5.
```

#### Parameters
* `data` Data to push

#### `public void `[`PushBack`](#class_azura_1_1_containers_1_1_vector_1af55839be4b5b355ed020a09201713935)`(Type && data)` {#class_azura_1_1_containers_1_1_vector_1af55839be4b5b355ed020a09201713935}

Appends data to the end of the vector.

The vector also doubles its size incase it fills up. Recommended to pre-allocate as much as you can.

Example: 
```cpp
Containers::Vector<int> arr(5, allocator);
// The vector is of size 0. The capacity is 5.

arr.PushBack(1);
// The vector is of size 1. The capacity is 5.

arr.PushBack(2);
// The vector is of size 2. The capacity is 5.

arr.PushBack(3);
// The vector is of size 3. The capacity is 5.

arr.PushBack(4);
// The vector is of size 4. The capacity is 5.
```

#### Parameters
* `data` Data to push

#### `public template<>`  <br/>`void `[`EmplaceBack`](#class_azura_1_1_containers_1_1_vector_1aadfac4f55765e41e812160022a9a949f)`(Args ... args)` {#class_azura_1_1_containers_1_1_vector_1aadfac4f55765e41e812160022a9a949f}

Appends data to the end of the vector by emplacing (like regular vector)

The vector also doubles its size incase it fills up. Recommended to pre-allocate as much as you can.

The arguments are the initialization parameters for the Type.

Example: 
```cpp
MyClass {
public:
  MyClass(int a) : data(a) {}

private:
  int data;
};

Containers::Vector<MyClass> arr(5, allocator);
// The vector is of size 0. The capacity is 5.

arr.EmplaceBack(1); // Creates a MyClass{1} at the memory location
// The vector is of size 1. The capacity is 5.

arr.EmplaceBack(2); // Creates a MyClass{2} at the memory location
// The vector is of size 2. The capacity is 5.
```

#### Parameters
* `args` Arguments to push

#### Parameters
* `Args` Arguments

#### `public void `[`PopBack`](#class_azura_1_1_containers_1_1_vector_1ad181b9dc858e1ad59fc64b39f795fb75)`()` {#class_azura_1_1_containers_1_1_vector_1ad181b9dc858e1ad59fc64b39f795fb75}

Removes the last element in the vector array.

#### `public int `[`FindFirst`](#class_azura_1_1_containers_1_1_vector_1a5da3557964b80e7c40a574fbbb7e24a6)`(const Type & data)` {#class_azura_1_1_containers_1_1_vector_1a5da3557964b80e7c40a574fbbb7e24a6}

Searches for the data in the vector.

#### Parameters
* `data` Data to search for

#### Returns
Index if Found, else -1

#### `public void `[`Remove`](#class_azura_1_1_containers_1_1_vector_1a4d191b0d876f03c44c5a58e6380a07ee)`(const Type & data)` {#class_azura_1_1_containers_1_1_vector_1a4d191b0d876f03c44c5a58e6380a07ee}

Searches for the given data in the vector and removes it.

#### Parameters
* `data` Data to Search for and Remove

#### `public void `[`Reserve`](#class_azura_1_1_containers_1_1_vector_1af53da95e8dee524b3b12f2208079741f)`(U32 requiredSize)` {#class_azura_1_1_containers_1_1_vector_1af53da95e8dee524b3b12f2208079741f}

Reserves a contiguous block for the vector.

Use this when you didn't provide an initial size for the vector. The growth of the vector is not controlled by this. Using Reserve **after** storing the data in the vector will lead an undefined behavior. Use this only once.

Example: 
```cpp
Containers::Vector<int> arr(allocator);
// The vector is still of size 0. The capacity is 0.

// ...
// Later in code
// ...

arr.Reserve(5);
// This allocates a 5 sized int vector
// The vector is still of size 0. The capacity is 5.
```

#### Parameters
* `requiredSize` Required Size

#### `public void `[`Resize`](#class_azura_1_1_containers_1_1_vector_1a6d651ba60555cf392c766eb77758bd81)`(U32 requiredSize)` {#class_azura_1_1_containers_1_1_vector_1a6d651ba60555cf392c766eb77758bd81}

Reserves a contiguous block for the vector and sets the size to the value specified.

Also, sets the size to the specified value. Use this when you didn't provide an initial size for the vector. The growth of the vector is not controlled by this. Using Resize **after** storing the data in the vector will lead an undefined behavior. Use this only once.

Example: 
```cpp
Containers::Vector<int> arr(allocator);
// The vector is still of size 0. The capacity is 0.

// ...
// Later in code
// ...

arr.Resize(5);
// This allocates a 5 sized int vector
// The vector is now of size 5. The capacity is 5.
```

#### Parameters
* `requiredSize` Required Size

#### `public bool `[`IsEmpty`](#class_azura_1_1_containers_1_1_vector_1a3f955f930eefbb486ff9880421bbc7a4)`() const` {#class_azura_1_1_containers_1_1_vector_1a3f955f930eefbb486ff9880421bbc7a4}

Checks if the container is empty.

#### Returns
`true` if empty

#### `public void `[`InsertAt`](#class_azura_1_1_containers_1_1_vector_1a9f350f9bbc7f5aaec6318369444d2d99)`(U32 idx,const Type & data)` {#class_azura_1_1_containers_1_1_vector_1a9f350f9bbc7f5aaec6318369444d2d99}

Inserts the supplied data at the index.

> Todo: Better Documentation

#### Parameters
* `idx` Target Index 

* `data` Data to insert

#### `public Type * `[`Data`](#class_azura_1_1_containers_1_1_vector_1a6e3fd44d2821bb97f170b6e5443ec57e)`()` {#class_azura_1_1_containers_1_1_vector_1a6e3fd44d2821bb97f170b6e5443ec57e}

Gets the Data pointer.

Example: 
```cpp
Containers::Vector<int> arr(5, allocator);
// The vector is still of size 0. The capacity is 5.

arr.Data() // is of type int*
```

#### `public const Type * `[`Data`](#class_azura_1_1_containers_1_1_vector_1a512f49e419efb7f312d0a08ddfc675b7)`() const` {#class_azura_1_1_containers_1_1_vector_1a512f49e419efb7f312d0a08ddfc675b7}

Gets the Data pointer.

Example: 
```cpp
Containers::Vector<int> arr(5, allocator);
// The vector is still of size 0. The capacity is 5.

arr.Data() // is of type int*
```
 Useful as a constant pointer when compared to its other overload.

#### `public void `[`Reset`](#class_azura_1_1_containers_1_1_vector_1a9a0fe84cbf357050432a0f851f8a8daa)`()` {#class_azura_1_1_containers_1_1_vector_1a9a0fe84cbf357050432a0f851f8a8daa}

Empties the vector.

But, it doesn't deallocate anything. It is meant to reuse the allocated memory.

Example: 
```cpp
Containers::Vector<int> arr(5, allocator);
// The vector is of size 0. The capacity is 5.

arr.PushBack(1);
// The vector is of size 1. The capacity is 5.

arr.PushBack(2);
// The vector is of size 2. The capacity is 5.

arr.PushBack(3);
// The vector is of size 3. The capacity is 5.

arr.PushBack(4);
// The vector is of size 4. The capacity is 5.

arr.Reset(); // Or Clear();
// The vector is of size 0. The capacity is 5.
```

#### `public void `[`Clear`](#class_azura_1_1_containers_1_1_vector_1acf0e0039895ef1262e9060406863351e)`()` {#class_azura_1_1_containers_1_1_vector_1acf0e0039895ef1262e9060406863351e}

[Vector::Reset()](#class_azura_1_1_containers_1_1_vector_1a9a0fe84cbf357050432a0f851f8a8daa)

[Vector::Reset()](#class_azura_1_1_containers_1_1_vector_1a9a0fe84cbf357050432a0f851f8a8daa)

#### `public Type & `[`operator[]`](#class_azura_1_1_containers_1_1_vector_1a9375fcc59ae44bfe9960d045d322d897)`(U32 idx)` {#class_azura_1_1_containers_1_1_vector_1a9375fcc59ae44bfe9960d045d322d897}

#### `public Type & `[`operator[]`](#class_azura_1_1_containers_1_1_vector_1aa88a35925d3013cd8050639e07938d62)`(U32 idx) const` {#class_azura_1_1_containers_1_1_vector_1aa88a35925d3013cd8050639e07938d62}

#### `public Type & `[`Last`](#class_azura_1_1_containers_1_1_vector_1aff0a689dcc4d698e49a40453f4e40d3b)`()` {#class_azura_1_1_containers_1_1_vector_1aff0a689dcc4d698e49a40453f4e40d3b}

#### `public Type & `[`Last`](#class_azura_1_1_containers_1_1_vector_1a6c87f384a7a0e3c4a76e53c8607e7f04)`() const` {#class_azura_1_1_containers_1_1_vector_1a6c87f384a7a0e3c4a76e53c8607e7f04}

#### `public inline U32 `[`GetSize`](#class_azura_1_1_containers_1_1_vector_1a109fab9164fcbf717f7dba81e1a8ac47)`() const` {#class_azura_1_1_containers_1_1_vector_1a109fab9164fcbf717f7dba81e1a8ac47}

#### `public inline U32 `[`GetMaxSize`](#class_azura_1_1_containers_1_1_vector_1ad51431e69d28f4f8e240c8f64cea17db)`() const` {#class_azura_1_1_containers_1_1_vector_1ad51431e69d28f4f8e240c8f64cea17db}

#### `public template<>`  <br/>`void `[`Assign`](#class_azura_1_1_containers_1_1_vector_1a41f8b0d08472230bbcaf4e9611c9a5b3)`(InputIt first,InputIt last)` {#class_azura_1_1_containers_1_1_vector_1a41f8b0d08472230bbcaf4e9611c9a5b3}

#### `public `[`Iterator`](#class_azura_1_1_containers_1_1_vector_1_1_iterator)` `[`Begin`](#class_azura_1_1_containers_1_1_vector_1a49e7eba07315528d8f75998944b0e7c5)`() const` {#class_azura_1_1_containers_1_1_vector_1a49e7eba07315528d8f75998944b0e7c5}

Returns an [Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator) pointing to the beginning of the vector. This is similar to [begin()](#class_azura_1_1_containers_1_1_vector_1a324d28755f053b4a83c89982eebeee93) of a std::vector.

#### Returns
[Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator)

#### `public `[`Iterator`](#class_azura_1_1_containers_1_1_vector_1_1_iterator)` `[`End`](#class_azura_1_1_containers_1_1_vector_1aaf8c1fcfb814e8030e5520b56c094c6a)`() const` {#class_azura_1_1_containers_1_1_vector_1aaf8c1fcfb814e8030e5520b56c094c6a}

Returns an [Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator) pointing to the end of the vector. This is similar to [end()](#class_azura_1_1_containers_1_1_vector_1a364c485414869aa9d6de832f2af0f46f) of a std::vector.

#### Returns
[Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator)

#### `public `[`Iterator`](#class_azura_1_1_containers_1_1_vector_1_1_iterator)` `[`begin`](#class_azura_1_1_containers_1_1_vector_1a324d28755f053b4a83c89982eebeee93)`() const` {#class_azura_1_1_containers_1_1_vector_1a324d28755f053b4a83c89982eebeee93}

Returns an [Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator) pointing to the beginning of the vector. This is similar to [begin()](#class_azura_1_1_containers_1_1_vector_1a324d28755f053b4a83c89982eebeee93) of a std::vector.

#### Returns
[Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator)

#### `public `[`Iterator`](#class_azura_1_1_containers_1_1_vector_1_1_iterator)` `[`end`](#class_azura_1_1_containers_1_1_vector_1a364c485414869aa9d6de832f2af0f46f)`() const` {#class_azura_1_1_containers_1_1_vector_1a364c485414869aa9d6de832f2af0f46f}

Returns an [Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator) pointing to the end of the vector. This is similar to [end()](#class_azura_1_1_containers_1_1_vector_1a364c485414869aa9d6de832f2af0f46f) of a std::vector.

#### Returns
[Iterator](#class_azura_1_1_containers_1_1_vector_1_1_iterator)

