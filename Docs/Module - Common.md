# group `Common` {#group___common}

Put here a longer description.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[`Azura::Memory::Allocator`](Docs/Module - Common.md#class_azura_1_1_memory_1_1_allocator) | Base class for an [Allocator](Docs/Module - Common.md#class_azura_1_1_memory_1_1_allocator).

# class `Azura::Memory::Allocator` {#class_azura_1_1_memory_1_1_allocator}

Base class for an [Allocator](#class_azura_1_1_memory_1_1_allocator).

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  `[`Allocator`](#class_azura_1_1_memory_1_1_allocator_1a63f66173b61bcb2936c5ee42e23d6555)`(void * resource,U32 size)` | 
`public  `[`Allocator`](#class_azura_1_1_memory_1_1_allocator_1a04e5f421a8ce8c63540f23ca0a6a16b4)`(AddressPtr resource,U32 size)` | 
`public virtual  `[`~Allocator`](#class_azura_1_1_memory_1_1_allocator_1a7ae01cfa01f9e4354c9d811d899aa1e1)`()` | 
`public  `[`Allocator`](#class_azura_1_1_memory_1_1_allocator_1a4f5ab0c255db815ab88fd0bbe7b7185a)`(const `[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` & other) = delete` | 
`public `[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` & `[`operator=`](#class_azura_1_1_memory_1_1_allocator_1ac64c3f6013af712213e377b9b21bf162)`(const `[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` & other) = delete` | 
`public  `[`Allocator`](#class_azura_1_1_memory_1_1_allocator_1a13a71cfb6bbfdb445a51db710597f1e3)`(`[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` && other) = default` | 
`public `[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` & `[`operator=`](#class_azura_1_1_memory_1_1_allocator_1a6bcad4fa8aff6a3b564da97634fdcdbf)`(`[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` && other) = default` | 
`public template<>`  <br/>`UniquePtr< Type > `[`New`](#class_azura_1_1_memory_1_1_allocator_1a93fef7c386893fb1feb03fab6dcb9fd2)`(Args... args)` | 
`public template<>`  <br/>`UniqueArrayPtr< Type > `[`NewArray`](#class_azura_1_1_memory_1_1_allocator_1af2d4ccebb175af0404080ab5caddaf1b)`(U32 numElements,Args... args)` | 
`public template<>`  <br/>`UniquePtr< Type > `[`RawNew`](#class_azura_1_1_memory_1_1_allocator_1aef78ca3a361d0374827a010cef7980fd)`(Args... args)` | 
`public template<>`  <br/>`UniqueArrayPtr< Type > `[`RawNewArray`](#class_azura_1_1_memory_1_1_allocator_1ab8ee7c90285ae60033b95a40e347e16d)`(U32 numElements,Args... args)` | 
`protected U32 `[`Size`](#class_azura_1_1_memory_1_1_allocator_1a3786ae37c581e3e8e221b51fa4c982e0)`() const` | 
`protected AddressPtr `[`BasePtr`](#class_azura_1_1_memory_1_1_allocator_1a45e1ae30db48775871f472d09f0ba354)`() const` | 
`protected void * `[`Allocate`](#class_azura_1_1_memory_1_1_allocator_1a4ea0bf858fb03e351e2b53cf533b28e0)`(U32 size,U32 alignment)` | 
`protected void `[`Deallocate`](#class_azura_1_1_memory_1_1_allocator_1aa11b99093ae5c589b86efaddcded4933)`(void * address)` | 
`protected virtual void `[`Reset`](#class_azura_1_1_memory_1_1_allocator_1a8e6578ce7e05f6f2d138f9e3385626cc)`()` | 

## Members

#### `public  `[`Allocator`](#class_azura_1_1_memory_1_1_allocator_1a63f66173b61bcb2936c5ee42e23d6555)`(void * resource,U32 size)` {#class_azura_1_1_memory_1_1_allocator_1a63f66173b61bcb2936c5ee42e23d6555}

#### `public  `[`Allocator`](#class_azura_1_1_memory_1_1_allocator_1a04e5f421a8ce8c63540f23ca0a6a16b4)`(AddressPtr resource,U32 size)` {#class_azura_1_1_memory_1_1_allocator_1a04e5f421a8ce8c63540f23ca0a6a16b4}

#### `public virtual  `[`~Allocator`](#class_azura_1_1_memory_1_1_allocator_1a7ae01cfa01f9e4354c9d811d899aa1e1)`()` {#class_azura_1_1_memory_1_1_allocator_1a7ae01cfa01f9e4354c9d811d899aa1e1}

#### `public  `[`Allocator`](#class_azura_1_1_memory_1_1_allocator_1a4f5ab0c255db815ab88fd0bbe7b7185a)`(const `[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` & other) = delete` {#class_azura_1_1_memory_1_1_allocator_1a4f5ab0c255db815ab88fd0bbe7b7185a}

#### `public `[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` & `[`operator=`](#class_azura_1_1_memory_1_1_allocator_1ac64c3f6013af712213e377b9b21bf162)`(const `[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` & other) = delete` {#class_azura_1_1_memory_1_1_allocator_1ac64c3f6013af712213e377b9b21bf162}

#### `public  `[`Allocator`](#class_azura_1_1_memory_1_1_allocator_1a13a71cfb6bbfdb445a51db710597f1e3)`(`[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` && other) = default` {#class_azura_1_1_memory_1_1_allocator_1a13a71cfb6bbfdb445a51db710597f1e3}

#### `public `[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` & `[`operator=`](#class_azura_1_1_memory_1_1_allocator_1a6bcad4fa8aff6a3b564da97634fdcdbf)`(`[`Allocator`](#class_azura_1_1_memory_1_1_allocator)` && other) = default` {#class_azura_1_1_memory_1_1_allocator_1a6bcad4fa8aff6a3b564da97634fdcdbf}

#### `public template<>`  <br/>`UniquePtr< Type > `[`New`](#class_azura_1_1_memory_1_1_allocator_1a93fef7c386893fb1feb03fab6dcb9fd2)`(Args... args)` {#class_azura_1_1_memory_1_1_allocator_1a93fef7c386893fb1feb03fab6dcb9fd2}

#### `public template<>`  <br/>`UniqueArrayPtr< Type > `[`NewArray`](#class_azura_1_1_memory_1_1_allocator_1af2d4ccebb175af0404080ab5caddaf1b)`(U32 numElements,Args... args)` {#class_azura_1_1_memory_1_1_allocator_1af2d4ccebb175af0404080ab5caddaf1b}

#### `public template<>`  <br/>`UniquePtr< Type > `[`RawNew`](#class_azura_1_1_memory_1_1_allocator_1aef78ca3a361d0374827a010cef7980fd)`(Args... args)` {#class_azura_1_1_memory_1_1_allocator_1aef78ca3a361d0374827a010cef7980fd}

#### `public template<>`  <br/>`UniqueArrayPtr< Type > `[`RawNewArray`](#class_azura_1_1_memory_1_1_allocator_1ab8ee7c90285ae60033b95a40e347e16d)`(U32 numElements,Args... args)` {#class_azura_1_1_memory_1_1_allocator_1ab8ee7c90285ae60033b95a40e347e16d}

#### `protected U32 `[`Size`](#class_azura_1_1_memory_1_1_allocator_1a3786ae37c581e3e8e221b51fa4c982e0)`() const` {#class_azura_1_1_memory_1_1_allocator_1a3786ae37c581e3e8e221b51fa4c982e0}

#### `protected AddressPtr `[`BasePtr`](#class_azura_1_1_memory_1_1_allocator_1a45e1ae30db48775871f472d09f0ba354)`() const` {#class_azura_1_1_memory_1_1_allocator_1a45e1ae30db48775871f472d09f0ba354}

#### `protected void * `[`Allocate`](#class_azura_1_1_memory_1_1_allocator_1a4ea0bf858fb03e351e2b53cf533b28e0)`(U32 size,U32 alignment)` {#class_azura_1_1_memory_1_1_allocator_1a4ea0bf858fb03e351e2b53cf533b28e0}

#### `protected void `[`Deallocate`](#class_azura_1_1_memory_1_1_allocator_1aa11b99093ae5c589b86efaddcded4933)`(void * address)` {#class_azura_1_1_memory_1_1_allocator_1aa11b99093ae5c589b86efaddcded4933}

#### `protected virtual void `[`Reset`](#class_azura_1_1_memory_1_1_allocator_1a8e6578ce7e05f6f2d138f9e3385626cc)`()` {#class_azura_1_1_memory_1_1_allocator_1a8e6578ce7e05f6f2d138f9e3385626cc}

