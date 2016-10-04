#include <iostream>
#include <vector>
#include <string>

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

    // Make the voxel sorter
    VoxelSorter sorter(config.binWidths.x, config.binWidths.y, config.binWidths.z, config.binOffsets.x, config.binOffsets.y, config.binOffsets.z);
    for (auto p : points)
    {
        auto located = sorter.identifyPoint(p);
        std::cout << p << " --> " << located.address << std::endl;
    }

    return 0;
}
