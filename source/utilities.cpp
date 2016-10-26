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
#include <stdexcept>

#include "json/json.h"
#include "vector3d.h"
#include "utilities.h"

Configuration LoadConfiguration(std::string fileName)
{
    Json::Value root;
    Json::Reader reader;
    std::ifstream configFile(fileName);
    std::string data((std::istreambuf_iterator<char>(configFile)), std::istreambuf_iterator<char>());

    auto parsingSuccessful = reader.parse(data, root, false);
    if (!parsingSuccessful)
    {
        throw std::invalid_argument("Failed to parse configuration file, :" + reader.getFormattedErrorMessages());
    }

    Configuration c;

    // Load the input file
    c.inputFile = root.get("input_file", "missing input file in config").asString();
    c.outputFile = root.get("output_file", "default_output.asc").asString();

    // Load the bin widths for i, j, and k
    double dx = root["voxel_space"].get("dx", 1).asDouble();
    double dy = root["voxel_space"].get("dy", 1).asDouble();
    double dz = root["voxel_space"].get("dz", 1).asDouble();
    c.binWidths = Vector3d(dx, dy, dz);

    // Load the bin offsets for i, j, and k
    double x0 = root["voxel_space"].get("x0", 0).asDouble();
    double y0 = root["voxel_space"].get("y0", 0).asDouble();
    double z0 = root["voxel_space"].get("z0", 0).asDouble();
    c.binOffsets = Vector3d(x0, y0, z0);

    // Load the thinning distance
    c.thinningDistance = root.get("thinning_distance", 0).asDouble();

    return c;
}

ParallelConfiguration LoadParallelConfiguration(std::string fileName)
{
    Json::Value root;
    Json::Reader reader;
    std::ifstream configFile(fileName);
    std::string data((std::istreambuf_iterator<char>(configFile)), std::istreambuf_iterator<char>());

    auto parsingSuccessful = reader.parse(data, root, false);
    if (!parsingSuccessful)
    {
        throw std::invalid_argument("Failed to parse configuration file, :" + reader.getFormattedErrorMessages());
    }

    ParallelConfiguration c;

    // Load the input file
    Json::Value inputFiles = root["input_files"];
    if (!inputFiles.isArray())
        throw "Configuration input files are invalid array";

    for (auto file : inputFiles)
    {
        c.inputFiles.push_back(file.asString());
    }

    c.outputFile = root.get("output_file", "default_output.asc").asString();

    // Load the bin widths for i, j, and k
    double dx = root["voxel_space"].get("dx", 1).asDouble();
    double dy = root["voxel_space"].get("dy", 1).asDouble();
    double dz = root["voxel_space"].get("dz", 1).asDouble();
    c.binWidths = Vector3d(dx, dy, dz);

    // Load the bin offsets for i, j, and k
    double x0 = root["voxel_space"].get("x0", 0).asDouble();
    double y0 = root["voxel_space"].get("y0", 0).asDouble();
    double z0 = root["voxel_space"].get("z0", 0).asDouble();
    c.binOffsets = Vector3d(x0, y0, z0);

    // Load the thinning distance
    c.thinningDistance = root.get("thinning_distance", 0).asDouble();

    return c;
}

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

void PrintConfigDetails(Configuration& config, int prefixSpace)
{
    std::string padding = std::string(prefixSpace, ' ');
    std::cout << padding << "input file:        " << config.inputFile << std::endl;
    std::cout << padding << "output file:       " << config.outputFile << std::endl;
    std::cout << padding << "voxel bin widths:  " << config.binWidths.Text() << std::endl;
    std::cout << padding << "voxel bin offsets: " << config.binOffsets.Text() << std::endl;
    std::cout << padding << "thinning distance: " << config.thinningDistance << std::endl;
}

void PrintConfigDetails(ParallelConfiguration& config, int prefixSpace)
{
    std::string padding = std::string(prefixSpace, ' ');

    for (auto v : config.inputFiles)
    {
        std::cout << padding << "input file:        " << v << std::endl;
    }

    std::cout << padding << "output file:       " << config.outputFile << std::endl;
    std::cout << padding << "voxel bin widths:  " << config.binWidths.Text() << std::endl;
    std::cout << padding << "voxel bin offsets: " << config.binOffsets.Text() << std::endl;
    std::cout << padding << "thinning distance: " << config.thinningDistance << std::endl;
}
