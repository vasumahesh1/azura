
.. _program_listing_file_Source_Dibella_Src_Materials_Primitives_Material.cpp:

Program Listing for File Material.cpp
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Src_Materials_Primitives_Material.cpp>` (``Source\Dibella\Src\Materials\Primitives\Material.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Material.h"
   #include "../../Interfaces/ImageHandler.h"
   
   namespace Dibella {
   Color3f Material::GetImageColor(const Point2f& uv, const ImageHandler* handle) {
     if (handle == nullptr) {
       return Color3f(1.0f);
     }
   
     return handle->GetPixel(UVToPixelSpace(uv, handle->GetWidth(), handle->GetHeight()));
   }
   }  // namespace Dibella
