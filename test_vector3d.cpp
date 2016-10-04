#include <gtest/gtest.h>
#include "vector3d.h"

TEST (ConstructorTest, EmptyTest)
{
  Vector3d empty;
  ASSERT_DOUBLE_EQ(empty.x, 0);
  ASSERT_DOUBLE_EQ(empty.y, 0);
  ASSERT_DOUBLE_EQ(empty.z, 0);
}

TEST (ConstructorTest, PositiveTest)
{
  Vector3d v(1.0, 1.0, 1.0);
  ASSERT_DOUBLE_EQ(v.x, 1.0);
  ASSERT_DOUBLE_EQ(v.y, 1.0);
  ASSERT_DOUBLE_EQ(v.z, 1.0);
}


TEST (ConstructorTest, NegativeTest)
{
  Vector3d v(-1.0, -1.0, -1.0);
  ASSERT_DOUBLE_EQ(v.x, -1.0);
  ASSERT_DOUBLE_EQ(v.y, -1.0);
  ASSERT_DOUBLE_EQ(v.z, -1.0);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
