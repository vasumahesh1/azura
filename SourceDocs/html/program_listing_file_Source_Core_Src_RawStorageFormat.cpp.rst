
.. _program_listing_file_Source_Core_Src_RawStorageFormat.cpp:

Program Listing for File RawStorageFormat.cpp
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Core_Src_RawStorageFormat.cpp>` (``Source\Core\Src\RawStorageFormat.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Core/RawStorageFormat.h"
   #include "Utils/Macros.h"
   
   #include <boost/preprocessor/stringize.hpp>
   
   namespace Azura {
   
   #define ENUM_SIZE_CASE(ENUM_TUPLE)                                                                                     \
     FORWARD_MAPPING(GET_FULL_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE), GET_FORMAT_WIDTH_FROM_TUPLE(ENUM_TUPLE))
   
   #define ENUM_ASPECT_DEPTH_CHECK(ENUM_TUPLE)                                                                            \
     case GET_FULL_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE):                                                                    \
       return (GET_ASPECT_MASK_FROM_TUPLE(ENUM_TUPLE) & AspectMaskBits::DepthBit) == AspectMaskBits::DepthBit;
   
   #define ENUM_ASPECT_STENCIL_CHECK(ENUM_TUPLE)                                                                            \
     case GET_FULL_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE):                                                                      \
       return (GET_ASPECT_MASK_FROM_TUPLE(ENUM_TUPLE) & AspectMaskBits::StencilBit) == AspectMaskBits::StencilBit;
   
   #define TO_STRING_MACRO(VAR) #VAR
   
   #define ENUM_TO_STRING_CASE(ENUM_TUPLE)                                                                                     \
     FORWARD_MAPPING(GET_FULL_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE), BOOST_PP_STRINGIZE(GET_FORMAT_NAME_FROM_TUPLE(ENUM_TUPLE)))
   
     
   
   U32 GetFormatSizeBits(RawStorageFormat format) {
     switch (format) {
       RAW_STORAGE_FORMAT_ITERATOR(ENUM_SIZE_CASE)
   
     default:
       return 0U;
     }
   }
   
   bool HasDepthComponent(RawStorageFormat format)
   {
     switch (format) {
       RAW_STORAGE_FORMAT_ITERATOR(ENUM_ASPECT_DEPTH_CHECK)
   
     default:
       return false;
     }
   }
   
   bool HasStencilComponent(RawStorageFormat format)
   {
     switch (format) {
       RAW_STORAGE_FORMAT_ITERATOR(ENUM_ASPECT_STENCIL_CHECK)
   
     default:
       return false;
     }
   }
   
   bool HasDepthOrStencilComponent(RawStorageFormat format)
   {
     return HasDepthComponent(format) || HasStencilComponent(format);
   }
   
   bool HasDepthAndStencilComponent(RawStorageFormat format)
   {
     return HasDepthComponent(format) || HasStencilComponent(format);
   }
   
   U32 GetFormatSize(RawStorageFormat format) {
     return GetFormatSizeBits(format) / 8;
   }
   
   String ToString(RawStorageFormat format)
   {
     switch (format) {
       RAW_STORAGE_FORMAT_ITERATOR(ENUM_TO_STRING_CASE)
   
     default:
       return "Invalid Format";
     }
   }
   
   }  // namespace Azura
