
.. _program_listing_file_Source_Common_Test_Hash_test.h:

Program Listing for File Hash_test.h
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Common_Test_Hash_test.h>` (``Source\Common\Test\Hash_test.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Utils/Hash.h"
   #include "gtest/gtest.h"
   
   using namespace Azura;
   
   class HashTest : public ::testing::Test {
    protected:
     HashTest() {}
   
     virtual ~HashTest() {}
   };
