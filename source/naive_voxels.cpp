#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdio>
#include <fstream>
#include <cmath>

#include "vector3d.h"
#include "utilities.h"
#include "voxelsorter.h"

void printUsageInstructions()
{
    std::cout << "naive_voxels: make sure to specify the config argument as a command line parameter" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printUsageInstructions();
        return -1;
    }

    std::cout << std::endl;
    std::cout << "naive_voxels: parsing configuration file " << argv[1] << std::endl;
    auto config = LoadConfiguration(argv[1]);
    PrintConfigDetails(config, 14);

    std::vector<Vector3d> points = LoadPointsFromFile(config.inputFile);
    std::cout << "naive_voxels: Loaded " << points.size() << " points from file." << std::endl;

    // Thin the points
    naiveThinning(points, config.thinningDistance);
    std::cout << "naive_voxels: Thinning completed, " << points.size() << " points remaining." << std::endl;

    // Make the voxel sorter
    VoxelSorter sorter(config.binWidths.x, config.binWidths.y, config.binWidths.z, config.binOffsets.x, config.binOffsets.y, config.binOffsets.z);

    // Make the sparse voxel representation as an unordered_map
    std::unordered_map<VoxelAddress, int> voxels;

    // Run through all of the points, determine their voxel address, and increment
    // the voxel intensity of that location
    for (auto p : points)
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
