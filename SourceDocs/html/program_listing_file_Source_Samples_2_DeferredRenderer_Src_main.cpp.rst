
.. _program_listing_file_Source_Samples_2_DeferredRenderer_Src_main.cpp:

Program Listing for File main.cpp
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Samples_2_DeferredRenderer_Src_main.cpp>` (``Source\Samples\2_DeferredRenderer\Src\main.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Log/Log.h"
   
   #include <iostream>
   #include "App/App.h"
   
   int main()
   {
     auto log_AppMain = Azura::Log("DeferredRenderer");
   
     try {
       Azura::App app;
       app.Initialize();
       app.Run();
       app.CleanUp();
     }
     catch(const std::runtime_error& e)
     {
       LOG_ERR(log_AppMain, LOG_LEVEL, e.what());
       std::cout << e.what();
     }
     return 0;
   }
