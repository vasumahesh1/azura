// --------------------------------------------------------------------------------------
// Copyright (c) 2018 - Nocturnal Studios
//
// All Rights Reserved.
//
// NOTICE:  All information contained herein is, and remains
// the property of Nocturnal Studios and its suppliers,
// if any.  The intellectual and technical concepts contained
// herein are proprietary to Nocturnal Studios
// and its suppliers and are protected by trade secret or copyright law.
// Dissemination of this information or reproduction of this material
// is strictly forbidden unless prior written permission is obtained
// from Nocturnal Studios.
//
// --------------------------------------------------------------------------------------

//
// Created by Vasu Mahesh on 7/21/2018.
//

#pragma once

#include <tuple>
#include "Types.h"
#include "Array.h"

namespace AZ {
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
    }

    template <U32 NumElements, typename... BatchProperties>
    class ArrayBatch {
        template <U32 Index>
        using Type = typename Impl::Get<Index, BatchProperties...>::Type;

    public:
      explicit ArrayBatch(Allocator* alloc);

      template <U32 Index>
      Array<Type<Index>, NumElements>& Get();

      template <U32 Index>
      const Array<Type<Index>, NumElements>& Get() const;

    private:
        std::tuple<Array<BatchProperties, NumElements>...> mData;
        SizeType mElements{NumElements};
    };

  template <U32 NumElements, typename ... BatchProperties>
  ArrayBatch<NumElements, BatchProperties...>::ArrayBatch(Allocator* alloc) : mData(Array<BatchProperties, NumElements>(alloc)...)
  {
  }

  template <U32 NumElements, typename ... BatchProperties>
  template <U32 Index>
  Array<typename Impl::Get<Index, BatchProperties...>::Type, NumElements>& ArrayBatch<NumElements, BatchProperties...>::Get()
  {
    return std::get<Index>(mData);
  }

  template <U32 NumElements, typename ... BatchProperties>
  template <U32 Index>
  const Array<typename Impl::Get<Index, BatchProperties...>::Type, NumElements>& ArrayBatch<NumElements, BatchProperties...>::Get() const
  {
    return std::get<Index>(mData);
  }
} // namespace AZ
