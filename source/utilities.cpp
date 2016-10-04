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

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream> // TODO: remove later
#include <iterator>

#include "vector3d.h"
#include "utilities.h"

std::vector<Vector3d> LoadPointsFromFile(std::string fileName)
{
    std::vector<Vector3d> loaded;
    std::ifstream workingFile(fileName);
    std::string workingLine;

    double sx, sy, sz;

    while (std::getline(workingFile, workingLine))
    {
        std::istringstream i(workingLine);
        std::vector<std::string> tokens{std::istream_iterator<std::string>(i), std::istream_iterator<std::string>()};

        if (tokens.size() < 3)
            continue;

        sx = std::stod(tokens[0]);
        sy = std::stod(tokens[1]);
        sz = std::stod(tokens[2]);

        loaded.push_back( Vector3d(sx, sy, sz) );
    }

    return loaded;
}
