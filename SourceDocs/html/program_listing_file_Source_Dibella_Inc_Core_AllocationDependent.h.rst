
.. _program_listing_file_Source_Dibella_Inc_Core_AllocationDependent.h:

Program Listing for File AllocationDependent.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_Core_AllocationDependent.h>` (``Source\Dibella\Inc\Core\AllocationDependent.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   namespace Dibella {
   class Allocator;
   
   class AllocationDependent {
    protected:
     explicit AllocationDependent(Allocator* alloc);
     Allocator* mAllocator;
   };
   
   inline AllocationDependent::AllocationDependent(Allocator* alloc) : mAllocator(alloc) {}
   }  // namespace Dibella
