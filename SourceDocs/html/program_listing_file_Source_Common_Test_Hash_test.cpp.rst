
.. _program_listing_file_Source_Common_Test_Hash_test.cpp:

Program Listing for File Hash_test.cpp
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Test_Hash_test.cpp>` (``Source\Common\Test\Hash_test.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "Hash_test.h"
   
   TEST(HashTest, FNVConstructs) {
     FNVHashOperator target = FNVHashOperator(0x811C9DC5, 0x1000193);
   }
   
   TEST(HashTest, FNVNormal) {
     FNVHashOperator target = FNVHashOperator(0x811C9DC5, 0x1000193);
     const char* str        = "engine_test_tools";
     const int len          = 17;
     auto result            = target.evaluate(str, len);
     ASSERT_EQ(result, U32(0x1741272c));
   }
   
   TEST(HashTest, FNVNormal2) {
     FNVHashOperator target = FNVHashOperator(0x811C9DC5, 0x1000193);
     const char* str        = "32f2389yt23t834kd9saksd93";
     auto result            = target.evaluate(str, 25);
     ASSERT_EQ(result, U32(0x1429f4ff));
   
     str    = "engine_test_tools";
     result = target.evaluate(str, 17);
     ASSERT_EQ(result, U32(0x1741272c));
   }
