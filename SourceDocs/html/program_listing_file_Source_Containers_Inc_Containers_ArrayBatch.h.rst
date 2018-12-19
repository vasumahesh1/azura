
.. _program_listing_file_Source_Containers_Inc_Containers_ArrayBatch.h:

Program Listing for File ArrayBatch.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Containers_Inc_Containers_ArrayBatch.h>` (``Source\Containers\Inc\Containers\ArrayBatch.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <tuple>
   #include "Array.h"
   #include "Types.h"
   
   namespace Azura {
   namespace Memory {
   class Allocator;
   }
   
   namespace Containers {
   namespace Impl {
   template <U32 Index, typename... BatchProperties>
   struct Get;
   
   template <typename First, typename... Rest>
   struct Get<0, First, Rest...> {
     Get() = delete;
     typedef First Type;
   };
   
   template <U32 Index, typename First, typename... Rest>
   struct Get<Index, First, Rest...> {
     Get() = delete;
     typedef typename Get<Index - 1, Rest...>::Type Type;
   };
   }  // namespace Impl
   
   template <U32 NumElements, typename... BatchProperties>
   class ArrayBatch {
     template <U32 Index>
     using Type = typename Impl::Get<Index, BatchProperties...>::Type;
   
    public:
     explicit ArrayBatch(Memory::Allocator &alloc);
   
     template <U32 Index>
     Array<Type<Index>, NumElements> &Get();
   
     template <U32 Index>
     const Array<Type<Index>, NumElements> &Get() const;
   
    private:
     std::tuple<Array<BatchProperties, NumElements>...> mData;
     SizeType mElements{NumElements};
   };
   
   template <U32 NumElements, typename... BatchProperties>
   ArrayBatch<NumElements, BatchProperties...>::ArrayBatch(Memory::Allocator &alloc)
       : mData(Array<BatchProperties, NumElements>(alloc)...) {}
   
   template <U32 NumElements, typename... BatchProperties>
   template <U32 Index>
   Array<typename Impl::Get<Index, BatchProperties...>::Type, NumElements>
       &ArrayBatch<NumElements, BatchProperties...>::Get() {
     return std::get<Index>(mData);
   }
   
   template <U32 NumElements, typename... BatchProperties>
   template <U32 Index>
   const Array<typename Impl::Get<Index, BatchProperties...>::Type, NumElements>
       &ArrayBatch<NumElements, BatchProperties...>::Get() const {
     return std::get<Index>(mData);
   }
   }  // namespace Containers
   }  // namespace Azura
