#include "gtest/gtest.h"

TEST(Test, Sanity) {
  EXPECT_EQ(1, 1);
}

int main(int argc, char **argv) {
  std::cout << "Testing Common Library" << std::endl;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
