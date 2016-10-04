#include <iostream>
#include <vector>
#include <string>

#include "vector3d.h"
#include "utilities.h"

int main()
{
    std::vector<Vector3d> v = LoadPointsFromFile("sample_data/sample.asc");

    std::cout << "Loaded " << v.size() << " points from file." << std::endl;

    return 0;
}
