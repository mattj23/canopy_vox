#include <cmath>
#include <gtest/gtest.h>
#include "voxelsorter.h"

TEST (VoxelAddressTest, EmptyConstructor)
{
    VoxelAddress empty;
    ASSERT_EQ(empty.i, 0);
    ASSERT_EQ(empty.j, 0);
    ASSERT_EQ(empty.k, 0);
}

TEST (VoxelAddressTest, ArgumentConstructor)
{
    VoxelAddress empty(1, 2, 3);
    ASSERT_EQ(empty.i, 1);
    ASSERT_EQ(empty.j, 2);
    ASSERT_EQ(empty.k, 3);
}

TEST (VoxelAddressTest, EqualityOperator)
{
    ASSERT_EQ(VoxelAddress(1,2,3), VoxelAddress(1,2,3));
    ASSERT_NE(VoxelAddress(0,2,3), VoxelAddress(1,2,3));
    ASSERT_NE(VoxelAddress(1,0,3), VoxelAddress(1,2,3));
    ASSERT_NE(VoxelAddress(1,2,0), VoxelAddress(1,2,3));
}

TEST (LocatedPointTest, Constructor)
{
    Vector3d v(1, 2, 3);
    VoxelAddress a(5, 6, 7);
    LocatedPoint p(v, a);
    ASSERT_EQ(Vector3d(1,2,3), p.location);
    ASSERT_EQ(VoxelAddress(5, 6, 7), p.address);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
