# BENCHMARKS

[Go Back](./)

## Benchmarks

### std::vector vs Containers::Vector

A point to note here, Azura vectors don't allocate memory on heap directly. It is forwarded to an Allocator. The following benchmark is using a `MemoryBuffer` on the Heap and using a `MonotonicAllocator`. MonotonicAllocators are sequential allocators that are really cheap and fast. It was no suprise here that my vector was able to perform better \(as it had no allocation overhead\). Later on, Tests for other allocators will also be present here.

#### Insert elements but reserve first

```cpp
// InsertWithReserve

// namespace std
std::vector<int> v;
v.reserve(U32(st.range(0)));

for (U32 i = 0; i < U32(st.range(0)); ++i)
{
  v.push_back(i);
}

// namespace Azura::Containers
Vector<int> v(U32(st.range(0)), alloc);

for (U32 i = 0; i < U32(st.range(0)); ++i)
{
  v.PushBack(i);
}

alloc.Reset();
```

![](https://github.com/vasumahesh1/azura/tree/743ccab790f1e97def9880afa64dbf2d3da2d70b/Docs/Images/InsertWithResize.PNG)

#### Insert lots of elements but reserve first

```cpp
// InsertWithReserveHeavy

// namespace std
for (U32 size = 0; size < U32(st.range(0)); ++size)
{
  std::vector<int> v;
  v.reserve(size);

  for (U32 i = 0; i < size; ++i) {
    v.push_back(i);
  }
}

// namespace Azura::Containers
for (U32 size = 0; size < U32(st.range(0)); ++size)
{
  Vector<int> v(size, alloc);

  for (U32 i = 0; i < size; ++i) {
    v.PushBack(i);
  }

  alloc.Reset();
}
```

![](https://github.com/vasumahesh1/azura/tree/743ccab790f1e97def9880afa64dbf2d3da2d70b/Docs/Images/InsertWithResizeHeavy.PNG)

