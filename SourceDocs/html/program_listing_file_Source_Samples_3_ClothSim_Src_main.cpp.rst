
.. _program_listing_file_Source_Samples_3_ClothSim_Src_main.cpp:

Program Listing for File main.cpp
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Samples_3_ClothSim_Src_main.cpp>` (``Source\Samples\3_ClothSim\Src\main.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "ClothSim/AppRenderer.h"
   #include "Log/Log.h"
   
   #include <iostream>
   
   int main()
   {
     auto log_AppMain = Azura::Log("ClothSim");
   
     try {
       Azura::AppRenderer app;
       app.Initialize();
       app.Run();
       app.Destroy();
     }
     catch(const std::runtime_error& e)
     {
       LOG_ERR(log_AppMain, LOG_LEVEL, e.what());
       std::cout << e.what();
     }
     return 0;
   }
