
.. _program_listing_file_Source_Sandbox_main.cpp:

Program Listing for File main.cpp
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Sandbox_main.cpp>` (``Source\Sandbox\main.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include <Containers/Vector.h>
   #include "Memory/MemoryFactory.h"
   #include "Memory/StackMemoryBuffer.h"
   #include "Memory/MonotonicAllocator.h"
   #include "Log/Log.h"
   
   #include <GLFW/glfw3.h>
   
   using namespace Azura::Containers; // NOLINT
   using namespace Azura; // NOLINT
   
   int main() {
     Log myLogger = Log("VulkanRenderSystem");
   
     LOG_INF(myLogger, 75, "Testing this Beautiful Printf like Log %d %f", 45, 5.0f);
     LOG_DBG(myLogger, 75, "Debug Message");
     LOG_INF(myLogger, 75, "Info Message");
     LOG_WRN(myLogger, 75, "Warning Message");
     LOG_ERR(myLogger, 75, "Error Message");
     LOG_FTL(myLogger, 75, "Fatal Message");
   
   
     // STACK_ALLOCATOR(Temporary, Memory::MonotonicAllocator, 2048);
     //
     // Vector<const char *> v(2, allocatorTemporary);
     // v.PushBack("Vasu");
     // v.PushBack("Mahesh");
     //
     // printf("%s \n", v[0]); // NOLINT
     // printf("%s \n", v[1]); // NOLINT
     //
     // const char** c = v.Data();
     // printf("%s \n", c[0]); // NOLINT
     // printf("%s \n", c[1]); // NOLINT
   
     if (glfwInit() == GLFW_FALSE) {
       exit(EXIT_FAILURE);
     }
     
     GLFWwindow* window = glfwCreateWindow(640, 480, "Simple example", nullptr, nullptr);
     
     if (window == nullptr) {
       glfwTerminate();
       exit(EXIT_FAILURE);
     }
     
     while (glfwWindowShouldClose(window) == GLFW_FALSE) {
       glfwPollEvents();
     }
     
     glfwDestroyWindow(window);
     glfwTerminate();
     exit(EXIT_SUCCESS);
   }
