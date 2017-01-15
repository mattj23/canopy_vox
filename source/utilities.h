/*
This file is part of Canopy_Vox, the Parallel Forest Canopy Voxel Generator.

Canopy_Vox is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Canopy_Vox is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License v3
along with Canopy_Vox.  If not, see <http://www.gnu.org/licenses/>.

Copyright (C) 2016  Matthew Jarvis

*/
#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <string>
#include "vector3d.h"


struct Configuration
{
    std::string inputFile;
    std::string outputFile;
    Vector3d binWidths;
    Vector3d binOffsets;
    double thinningDistance;
};

struct ParallelConfiguration
{
    std::vector<std::string> inputFiles;
    std::string scratchDirectory;
    double voxelDistance;
    double binningDistance;
    double thinningDistance;
    bool debug;
};

std::vector<Vector3d> LoadPointsFromFile(std::string fileName);

Configuration LoadConfiguration(std::string fileName);
ParallelConfiguration LoadParallelConfiguration(std::string fileName);

void PrintConfigDetails(Configuration& config, int prefixSpace);
void PrintConfigDetails(ParallelConfiguration& config, int prefixSpace);

#endif
