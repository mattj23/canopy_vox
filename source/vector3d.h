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

Copyright (C) 2013-2016  Matthew Jarvis

	This vector library defines a class that represents a vector in R3 space and
	defines basic operations relating to that type, including addition, scalar
	multiplication, dot products, distances, and lengths.

*/

#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <iostream>

class Vector3d
{
public:
    double x;
    double y;
    double z;

    Vector3d();
    Vector3d(double x_, double y_, double z_);

    // Returns the vector rotated about the x-axis by an angle in radians
    Vector3d RotateAboutX(double angle) const;

    // Returns the vector rotated about the y-axis by an angle in radians
    Vector3d RotateAboutY(double angle) const;

    // Returns the vector rotated about the z-axis by an angle in radians
    Vector3d RotateAboutZ(double angle) const;

    // Returns a normalized unit vector
    Vector3d Unit(void) const;

    // Returns the dot product of this vector and another vector
    double Dot(const Vector3d& other) const;

    // Returns the distance between the end of this vector and another vector
    double DistanceTo(const Vector3d& other) const;

    // Returns the length of this vector
    double Length(void) const;

    // Returns the angle between this vector and another vector
    double AngleTo(const Vector3d& other) const;

    // Operators
    Vector3d operator+(const Vector3d& other) const;
    Vector3d operator-(const Vector3d& other) const;
    Vector3d operator*(const double& scale) const;
    bool operator==(const Vector3d& other) const;

    // Text
    std::string Text(void);
};

// Vector3d ostream printing operator
::std::ostream& operator<<(::std::ostream& os, const Vector3d& v);

// Returns the angle between two vectors in radians
double AngleBetweenVectors(Vector3d v0, Vector3d v1);

// Performs naive, near O(n^2) thinning of a list of points by removing all
// redundant points within the thinningDistance, starting at the beginning
// of the vector and working its way to the end.
void naiveThinning(std::vector<Vector3d>& points, double thinningDistance);

#endif // VECTOR3D_H
