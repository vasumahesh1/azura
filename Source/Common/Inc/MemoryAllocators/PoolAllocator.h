#pragma once

#include <stdlib.h>
#include <iostream>
#include "Types.h"

namespace AZ {
  template<typename T>
    struct PoolNode {
      T mData;
      PoolNode* pNext{nullptr};
    };

  template<typename T>
    class PoolAllocator {
      using Node = PoolNode<T>;
      public:
      PoolAllocator(U32 totalElements);
      ~PoolAllocator();

      T* allocate();
      void deallocate(T* data);

      private:
      U32 mCount;
      Node* pHead;
      Node* pTail;
      void* pMemory;
    };


  template<typename T>
    PoolAllocator<T>::PoolAllocator(U32 totalElements) : mCount(totalElements) {
      pMemory = malloc(sizeof(Node) * mCount);
      pHead = (Node*) pMemory;

      Node* itr = pHead;
      Node* prev = nullptr;

      // Form Initial Links between Nodes
      for (auto i = 0U; i < mCount; i++) {
        itr->pNext = nullptr;

        if (prev != nullptr) {
          prev->pNext = itr;
        }

        prev = itr;
        itr++;
      }

      pTail = prev;
    }

  template<typename T>
    PoolAllocator<T>::~PoolAllocator() {
      free(pMemory);
    }

  template<typename T>
    T* PoolAllocator<T>::allocate() {
      T* result = nullptr;
      if (pHead != nullptr) {
        result = reinterpret_cast<T*>(pHead);

        auto oldNode = pHead;
        pHead = pHead->pNext;
        oldNode->pNext = nullptr;

        if (pHead == nullptr) {
          pTail = nullptr;
        }
      }

      return result;
    }


  template<typename T>
    void PoolAllocator<T>::deallocate(T* data) {
      Node* node = reinterpret_cast<Node*>(data);
      if (pTail != nullptr) {
        pTail->pNext = node;
      } else {
        pHead = node;
      }

      pTail = node;
    }

} // namespace AZ
