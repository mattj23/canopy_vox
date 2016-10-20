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
    std::cout << "kdtree_voxels: make sure to specify the config argument as a command line parameter" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printUsageInstructions();
        return -1;
    }

    std::cout << std::endl;
    std::cout << "kdtree_voxels: parsing configuration file " << argv[1] << std::endl;
    auto config = LoadConfiguration(argv[1]);
    PrintConfigDetails(config, 14);

    PointCloud cloud(LoadPointsFromFile(config.inputFile));
    std::cout << "kdtree_voxels: Loaded " << cloud.pts.size() << " points from file." << std::endl;

    // Thin the points
    std::cout << "kdtree_voxels: building kd-tree index" << std::endl;
    typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<double, PointCloud> ,PointCloud,3> my_kd_tree_t;

    my_kd_tree_t index(3, cloud, KDTreeSingleIndexAdaptorParams(10));
    index.buildIndex();
    std::cout << "kdtree_voxels: index complete" << std::endl;
    std::cout << "kdtree_voxels: Thinning point cloud [0%]";

    std::set<size_t> removeIndicies;
    const double searchRadius = config.thinningDistance*config.thinningDistance;
    for (size_t i = 0; i < cloud.pts.size(); i++)
    {
        if (removeIndicies.find(i) == removeIndicies.end())
        {
            double query_pt[3] = { cloud.pts[i].x, cloud.pts[i].y, cloud.pts[i].z};

            std::vector<std::pair<size_t,double>> indices_dists;
            RadiusResultSet<double,size_t> resultSet(searchRadius, indices_dists);
            index.findNeighbors(resultSet, query_pt, nanoflann::SearchParams());

            for (auto r : resultSet.m_indices_dists)
            {
                if (i != r.first)
                    removeIndicies.insert(r.first);
            }
        }

        if (i % 500 == 0)
            std::cout << "\rkdtree_voxels: Thinning point cloud [" << (int)(i * 100 / cloud.pts.size()) << "%]";
    }

    cloud.RemoveAtIndicies(removeIndicies);
    std::cout << "\rkdtree_voxels: Thinning completed, " << cloud.pts.size() << " points remaining." << std::endl;

    std::cout << "kdtree_voxels: Sorting into voxels" << std::endl;

    // Make the voxel sorter
    VoxelSorter sorter(config.binWidths.x, config.binWidths.y, config.binWidths.z, config.binOffsets.x, config.binOffsets.y, config.binOffsets.z);

    // Make the sparse voxel representation as an unordered_map
    std::unordered_map<VoxelAddress, int> voxels;

    // Run through all of the points, determine their voxel address, and increment
    // the voxel intensity of that location
    for (auto p : cloud.pts)
    {
        auto located = sorter.identifyPoint(p);
        incrementVoxelIntensity(voxels, located.address);
    }

    // Attempt to remove the output file
    std::remove(config.outputFile.c_str());

    // Open the output file
    std::ofstream outfile;
    outfile.open(config.outputFile, std::ios_base::app);

    // Print out the addresses and intensities
    for (auto voxel : voxels)
    {
        outfile << voxel.first.i << "," << voxel.first.j << "," << voxel.first.k << "," << voxel.second << std::endl;
    }

    return 0;
}
