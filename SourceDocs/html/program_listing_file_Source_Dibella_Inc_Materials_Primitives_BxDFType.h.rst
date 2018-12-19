
.. _program_listing_file_Source_Dibella_Inc_Materials_Primitives_BxDFType.h:

Program Listing for File BxDFType.h
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Dibella_Inc_Materials_Primitives_BxDFType.h>` (``Source\Dibella\Inc\Materials\Primitives\BxDFType.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   namespace Dibella {
   enum class BxDFType
   {
     // This BxDF handles rays that are reflected off surfaces
     BsdfReflection = 1 << 0,
   
     // This BxDF handles rays that are transmitted through surfaces
     BsdfTransmission = 1 << 1,
   
     // This BxDF represents diffuse energy scattering, which is uniformly random
     BsdfDiffuse = 1 << 2,
   
     // This BxDF represents glossy energy scattering, which is biased toward certain directions
     BsdfGlossy = 1 << 3,
   
     // This BxDF handles specular energy scattering, which has no element of randomness
     BsdfSpecular = 1 << 4,
     BsdfAll      = BsdfDiffuse | BsdfGlossy | BsdfSpecular | BsdfReflection | BsdfTransmission
   };
   }
