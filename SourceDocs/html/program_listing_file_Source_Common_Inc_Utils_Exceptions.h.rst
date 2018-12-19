
.. _program_listing_file_Source_Common_Inc_Utils_Exceptions.h:

Program Listing for File Exceptions.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Inc_Utils_Exceptions.h>` (``Source\Common\Inc\Utils\Exceptions.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <stdexcept>
   
   namespace Azura {
   
   class NotImplementedException : public std::logic_error {
    public:
     virtual char const *what() const {
       return "Function or branch not yet implemented.";
     }
   };
   
   }  // namespace Azura
