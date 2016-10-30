#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdio>
#include <fstream>
#include <cmath>
#include <set>

#include "nanoflann.hpp"
#include "vector3d.h"
#include "pointcloud.h"
#include "utilities.h"
#include "voxelsorter.h"

using namespace nanoflann;

void printUsageInstructions()
{
    std::cout << "closest_point_check: make sure to specify the input asc file as a command line parameter" << std::endl;
    std::cout << "                     This tool will search the contents of a point cloud .asc file and " << std::endl;
    std::cout << "                     find the distance between the closest two points.  The resulting  " << std::endl;
    std::cout << "                     value should be used as thinning tolerance for a global thinning and" << std::endl;
    std::cout << "                     voxelization." << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printUsageInstructions();
        return -1;
    }

    PointCloud cloud(LoadPointsFromFile(std::string(argv[1])));
    std::cout << "closest_point_check: Loaded " << cloud.pts.size() << " points from file." << std::endl;

    // Thin the points
    std::cout << "closest_point_check: building kd-tree index" << std::endl;
    typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<double, PointCloud> ,PointCloud,3> my_kd_tree_t;

    my_kd_tree_t index(3, cloud, KDTreeSingleIndexAdaptorParams(10));
    index.buildIndex();
    std::cout << "closest_point_check: index complete" << std::endl;

    double minDistance;
    for (size_t i = 0; i < cloud.pts.size(); i++)
    {
        double query_pt[3] = { cloud.pts[i].x, cloud.pts[i].y, cloud.pts[i].z};
        // find the nearest neighnor

    }

    std::cout << "closest_point_check: closest distance between points is " << minDistance << std::endl;
    return 0;
}
