#include <cmath>
#include <unordered_map>
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

TEST (VoxelSorterTest, SimpleSort)
{
    VoxelSorter sorter(1, 2, 3, 0, 0, 0);

    LocatedPoint p = sorter.identifyPoint(Vector3d(0, 0, 0));
    ASSERT_EQ(VoxelAddress(0, 0, 0), p.address);

    p = sorter.identifyPoint(Vector3d(0.5, 1.5, 2.5));
    ASSERT_EQ(VoxelAddress(0, 0, 0), p.address);

    p = sorter.identifyPoint(Vector3d(1.5, 2.5, 3.5));
    ASSERT_EQ(VoxelAddress(1, 1, 1), p.address);
}

TEST (VoxelSorterTest, FractionalBinSort)
{
    VoxelSorter sorter(0.5, 0.5, 0.5, 0, 0, 0);

    LocatedPoint p = sorter.identifyPoint(Vector3d(0, 0, 0));
    ASSERT_EQ(VoxelAddress(0, 0, 0), p.address);

    p = sorter.identifyPoint(Vector3d(0.65, 1.65, 2.65));
    ASSERT_EQ(VoxelAddress(1, 3, 5), p.address);
}

TEST (VoxelSorterTest, ShiftedSort)
{
    VoxelSorter sorter(1, 1, 1, 0.5, -1.5, -2.0);

    LocatedPoint p = sorter.identifyPoint(Vector3d(0, 0, 0));
    ASSERT_EQ(VoxelAddress(-1, 1, 2), p.address);

    p = sorter.identifyPoint(Vector3d(-3.2, 4.1, -4.2));
    ASSERT_EQ(VoxelAddress(-4, 5, -3), p.address);

    p = sorter.identifyPoint(Vector3d(1.5, 2.5, 3.5));
    ASSERT_EQ(VoxelAddress(1, 4, 5), p.address);
}

TEST (VoxelAddressTest, UnorderedMap)
{
    std::unordered_map<VoxelAddress, int> umap;
    umap[VoxelAddress(1, 2, 3)] = 1;
    umap[VoxelAddress(2, 3, 4)] = 2;
    umap[VoxelAddress(3, 4, 5)] = 3;

    ASSERT_EQ(2, umap[VoxelAddress(2,3,4)]);
}

TEST (VoxelBinning, IncrementIntensity)
{
    std::unordered_map<VoxelAddress, int> voxels;
    incrementVoxelIntensity(voxels, VoxelAddress(0, 0, 0));
    incrementVoxelIntensity(voxels, VoxelAddress(1, 0, 0));
    incrementVoxelIntensity(voxels, VoxelAddress(1, 0, 0));
    incrementVoxelIntensity(voxels, VoxelAddress(0, 1, 0));
    incrementVoxelIntensity(voxels, VoxelAddress(0, 1, 0));
    incrementVoxelIntensity(voxels, VoxelAddress(0, 1, 0));

    ASSERT_EQ(1, voxels[VoxelAddress(0,0,0)]);
    ASSERT_EQ(2, voxels[VoxelAddress(1,0,0)]);
    ASSERT_EQ(3, voxels[VoxelAddress(0,1,0)]);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
