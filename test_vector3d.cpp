#include <cmath>
#include <gtest/gtest.h>
#include "vector3d.h"

#define ZERO_TOL    0.000001
#define ZERO_TOL_2  0.00001

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

TEST(Vector3dTest, Operators)
{
  Vector3d v1(1.0, 1.0, 1.0);
  Vector3d v2(0.5, 0.5, 0.5);
  Vector3d v3(1.5, 1.5, 1.5);
  Vector3d t1(1, 0, 0);
  Vector3d t2(0, 1, 0);
  Vector3d t3(0, 0, 1);

  Vector3d r1 = v1 + v2;
  ASSERT_EQ(r1, v3);

  Vector3d r2 = v3 - v2;
  ASSERT_EQ(r2, v1);

  Vector3d r3 = v2 * 3;
  ASSERT_EQ(r3, v3);

  Vector3d r4 = t1 + t2 + t3;
  ASSERT_EQ(r4, v1);
}

TEST(Vector3dTest, UnitAndDotProducts)
{
  Vector3d t1(100, 0, 0);
  Vector3d t2(0, 100, 0);
  Vector3d t3(0, 0, 100);

  // Unit vector generation tests
  ASSERT_EQ( Vector3d(1, 0, 0), t1.Unit() );
  ASSERT_EQ( Vector3d(1 / sqrt(2.0), 1 / sqrt(2.0), 0), (t1 + t2).Unit());
  ASSERT_NEAR(1, (t1 + t2 + t3).Unit().Length(), ZERO_TOL);

  // Dot product tests
  Vector3d v0(1, 2, 3);
  Vector3d v1(4, 5, 6);
  ASSERT_EQ(v0.Dot(v1), 32);
}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
