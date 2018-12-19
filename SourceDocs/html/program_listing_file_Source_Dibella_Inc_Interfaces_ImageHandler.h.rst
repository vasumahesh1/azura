
.. _program_listing_file_Source_Dibella_Inc_Interfaces_ImageHandler.h:

Program Listing for File ImageHandler.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_Interfaces_ImageHandler.h>` (``Source\Dibella\Inc\Interfaces\ImageHandler.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "../Core/Types.h"
   
   namespace Dibella {
   class ImageHandler {
    public:
     virtual ~ImageHandler() = default;
   
     ImageHandler(const ImageHandler& other)     = default;
     ImageHandler(ImageHandler&& other) noexcept = default;
     ImageHandler& operator=(const ImageHandler& other) = default;
     ImageHandler& operator=(ImageHandler&& other) noexcept = default;
   
     virtual SizeType GetWidth() const                   = 0;
     virtual SizeType GetHeight() const                  = 0;
     virtual Color3f GetPixel(Point2i pixelCoords) const = 0;
   };
   }  // namespace Dibella
