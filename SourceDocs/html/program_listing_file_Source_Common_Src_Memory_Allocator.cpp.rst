
.. _program_listing_file_Source_Common_Src_Memory_Allocator.cpp:

Program Listing for File Allocator.cpp
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Src_Memory_Allocator.cpp>` (``Source\Common\Src\Memory\Allocator.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Memory/Allocator.h"
   
   namespace Azura {
   namespace Memory {
   
   MemoryRange::MemoryRange(U32 offset, U32 size) : m_offset(offset), m_size(size) {}
   
   Allocator::Allocator(void* resource, U32 size)
       // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
       : m_basePtr(reinterpret_cast<AddressPtr>(resource)), m_size(size) {}
   
   Allocator::Allocator(AddressPtr resource, U32 size) : m_basePtr(resource), m_size(size) {}
   
   Allocator::~Allocator() = default;
   
   void Allocator::Reset() {}
   
   U32 Allocator::Size() const {
     return m_size;
   }
   
   AddressPtr Allocator::BasePtr() const {
     return m_basePtr;
   }
   }  // namespace Memory
   }  // namespace Azura
