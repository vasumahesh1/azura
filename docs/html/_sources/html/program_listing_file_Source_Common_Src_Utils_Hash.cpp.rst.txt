
.. _program_listing_file_Source_Common_Src_Utils_Hash.cpp:

Program Listing for File Hash.cpp
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Src_Utils_Hash.cpp>` (``Source\Common\Src\Utils\Hash.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Utils/Hash.h"
   
   namespace Azura {
   HashOperator::HashOperator(SizeType seed) : mSeed(seed) {}
   
   FNVHashOperator::FNVHashOperator(SizeType seed) : HashOperator(seed) {}
   
   FNVHashOperator::FNVHashOperator(SizeType seed, SizeType prime) : HashOperator(seed), mPrime(prime) {}
   
   SizeType FNVHashOperator::evaluate(const void* bytes, U32 numBytes) const {
     return FNVHash(bytes, numBytes, mSeed, mPrime);
   }
   
   SizeType FNVHashOperator::evaluate(const char* bytes, U32 len) const {
     // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
     return evaluate(reinterpret_cast<const void*>(bytes), len);
   }
   
   SizeType FNVHashOperator::evaluate(const int& num) const {
     return evaluate(&num, sizeof(int));
   }
   }  // namespace Azura
