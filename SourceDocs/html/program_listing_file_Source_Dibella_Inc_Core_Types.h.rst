
.. _program_listing_file_Source_Dibella_Inc_Core_Types.h:

Program Listing for File Types.h
================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_Core_Types.h>` (``Source\Dibella\Inc\Core\Types.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <ToolsCore.h>
   #include <cstddef>
   #include <string>
   #include "mathfu/matrix.h"
   #include "mathfu/vector.h"
   
   namespace Dibella {
   using Color3f  = mathfu::Vector<float, 3>;
   using Normal3f = mathfu::Vector<float, 3>;
   using Vector4f = mathfu::Vector<float, 4>;
   using Vector3f = mathfu::Vector<float, 3>;
   using Point3f  = mathfu::Vector<float, 3>;
   using Vector2f = mathfu::Vector<float, 2>;
   using Point2f  = mathfu::Vector<float, 2>;
   using Vector3i = mathfu::Vector<int, 3>;
   using Point3i  = mathfu::Vector<int, 3>;
   using Vector2i = mathfu::Vector<int, 2>;
   using Point2i  = mathfu::Vector<int, 2>;
   using Float    = float;
   
   using Matrix3f = mathfu::Matrix<float, 3, 3>;
   using Matrix4f = mathfu::Matrix<float, 4, 4>;
   
   using DString = std::string;
   
   using SizeType = std::size_t;
   
   using U64 = EngineTools::U64;
   }  // namespace Dibella
