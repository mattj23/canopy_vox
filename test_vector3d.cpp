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

TEST(Vector3dTest, VectorRotations)
{
  Vector3d v(1, 1, 1);
  ASSERT_EQ(v.RotateAboutX(-4.712385), Vector3d(1,-1,1));
  ASSERT_EQ(v.RotateAboutY(-4.712385), Vector3d(1,1,-1));
  ASSERT_EQ(v.RotateAboutZ(-4.712385), Vector3d(-1,1,1));
  ASSERT_EQ(v.RotateAboutX(-3.141590), Vector3d(1,-1,-1));
  ASSERT_EQ(v.RotateAboutY(-3.141590), Vector3d(-1,1,-1));
  ASSERT_EQ(v.RotateAboutZ(-3.141590), Vector3d(-1,-1,1));
  ASSERT_EQ(v.RotateAboutX(-1.570795), Vector3d(1,1,-1));
  ASSERT_EQ(v.RotateAboutY(-1.570795), Vector3d(-1,1,1));
  ASSERT_EQ(v.RotateAboutZ(-1.570795), Vector3d(1,-1,1));
  ASSERT_EQ(v.RotateAboutX(0.000000), Vector3d(1,1,1));
  ASSERT_EQ(v.RotateAboutY(0.000000), Vector3d(1,1,1));
  ASSERT_EQ(v.RotateAboutZ(0.000000), Vector3d(1,1,1));
  ASSERT_EQ(v.RotateAboutX(1.570795), Vector3d(1,-1,1));
  ASSERT_EQ(v.RotateAboutY(1.570795), Vector3d(1,1,-1));
  ASSERT_EQ(v.RotateAboutZ(1.570795), Vector3d(-1,1,1));
  ASSERT_EQ(v.RotateAboutX(3.141590), Vector3d(1,-1,-1));
  ASSERT_EQ(v.RotateAboutY(3.141590), Vector3d(-1,1,-1));
  ASSERT_EQ(v.RotateAboutZ(3.141590), Vector3d(-1,-1,1));
  ASSERT_EQ(v.RotateAboutX(4.712385), Vector3d(1,1,-1));
  ASSERT_EQ(v.RotateAboutY(4.712385), Vector3d(-1,1,1));
  ASSERT_EQ(v.RotateAboutZ(4.712385), Vector3d(1,-1,1));
  ASSERT_EQ(v.RotateAboutX(6.283180), Vector3d(1,1,1));
  ASSERT_EQ(v.RotateAboutY(6.283180), Vector3d(1,1,1));
  ASSERT_EQ(v.RotateAboutZ(6.283180), Vector3d(1,1,1));
}

TEST(Vector3dTest, VectorAngles)
{
  // checking against http://www.usbe.cas.cz/people/lazar/celler/online_tools.php
  ASSERT_NEAR( 114.29 * 3.14159 / 180.0, Vector3d(0.25,-0.57,-0.91).AngleTo( Vector3d(-0.18,-0.16,0.21 ) ), 0.001);
  ASSERT_NEAR( 17.7   * 3.14159 / 180.0, Vector3d(-0.70,0.18,0.88).AngleTo(  Vector3d(-0.67,-0.16,0.82 ) ), 0.001);
  ASSERT_NEAR( 116.72 * 3.14159 / 180.0, Vector3d(-0.69,0.04,0.48).AngleTo(  Vector3d(0.47,-0.99,-0.11)  ), 0.001);
  ASSERT_NEAR( 69.78  * 3.14159 / 180.0, Vector3d(-0.97,0.00,0.03).AngleTo(  Vector3d(-0.31,-0.74,0.49 ) ), 0.001);
  ASSERT_NEAR( 26.58  * 3.14159 / 180.0, Vector3d(0.87,-0.82,-0.17).AngleTo( Vector3d(0.45,-0.79,0.20 )  ), 0.001);
  ASSERT_NEAR( 37.36  * 3.14159 / 180.0, Vector3d(0.29,-0.82,-0.63).AngleTo( Vector3d(0.02,-0.17,-0.55)  ), 0.001);
  ASSERT_NEAR( 106.56 * 3.14159 / 180.0, Vector3d(0.78,-0.81,-0.50).AngleTo( Vector3d(0.24,0.63,-0.16)   ), 0.001);
  ASSERT_NEAR( 139.83 * 3.14159 / 180.0, Vector3d(0.36,0.83,-0.94).AngleTo(  Vector3d(-0.85,-0.45,0.37 ) ), 0.001);
  ASSERT_NEAR( 100.5  * 3.14159 / 180.0, Vector3d(-0.32,0.31,-0.07).AngleTo( Vector3d(0.13,-0.16,-0.58)  ), 0.001);
  ASSERT_NEAR( 84.07  * 3.14159 / 180.0, Vector3d(-0.80,-0.86,0.27).AngleTo( Vector3d(0.37,-0.30,0.44 )  ), 0.001);
  ASSERT_NEAR( 148.22 * 3.14159 / 180.0, Vector3d(-0.46,-0.26,0.53).AngleTo( Vector3d(0.83,-0.22,-0.78)  ), 0.001);
  ASSERT_NEAR( 150.64 * 3.14159 / 180.0, Vector3d(0.48,-0.90,-0.91).AngleTo( Vector3d(-0.74,0.31,0.68 )  ), 0.001);
  ASSERT_NEAR( 80.35  * 3.14159 / 180.0, Vector3d(0.26,0.33,-0.01).AngleTo(  Vector3d(-0.84,0.97,0.49 )  ), 0.001);
  ASSERT_NEAR( 118.86 * 3.14159 / 180.0, Vector3d(0.03,0.95,-0.16).AngleTo(  Vector3d(0.03,-0.47,-0.59)  ), 0.001);
  ASSERT_NEAR( 100.62 * 3.14159 / 180.0, Vector3d(0.76,0.77,-0.87).AngleTo(  Vector3d(0.59,0.00,0.81 )   ), 0.001);
  ASSERT_NEAR( 52.9   * 3.14159 / 180.0, Vector3d(0.41,0.80,-0.60).AngleTo(  Vector3d(-0.46,0.67,-0.41)  ), 0.001);
  ASSERT_NEAR( 138.96 * 3.14159 / 180.0, Vector3d(0.26,-0.09,0.72).AngleTo(  Vector3d(0.12,0.50,-0.65)   ), 0.001);
  ASSERT_NEAR( 98.14  * 3.14159 / 180.0, Vector3d(-0.54,0.14,0.88).AngleTo(  Vector3d(0.01,-0.72,0.00 )  ), 0.001);
  ASSERT_NEAR( 117.58 * 3.14159 / 180.0, Vector3d(0.26,0.85,-0.01).AngleTo(  Vector3d(0.73,-0.72,-0.02)  ), 0.001);
  ASSERT_NEAR( 95.13  * 3.14159 / 180.0, Vector3d(0.93,-0.39,-0.33).AngleTo( Vector3d(-0.35,-0.64,-0.02) ), 0.001);

}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
