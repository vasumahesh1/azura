
.. _program_listing_file_Source_Common_Test_test.cpp:

Program Listing for File test.cpp
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Test_test.cpp>` (``Source\Common\Test\test.cpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #include "gtest/gtest.h"
   
   TEST(Test, Sanity) {
     EXPECT_EQ(1, 1);
   }
   
   int main(int argc, char **argv) {
     std::cout << "Testing Common Library" << std::endl;
     ::testing::InitGoogleTest(&argc, argv);
     return RUN_ALL_TESTS();
   }
