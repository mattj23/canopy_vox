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

Copyright (C) 2013  Matthew Jarvis

	This vector library defines a class that represents a vector in R3 space and
	defines basic operations relating to that type, including addition, scalar
	multiplication, dot products, distances, and lengths.

*/

#include "vector3d.h"
#include <cmath>
#include <vector>
#include <tuple>
#include <algorithm>

#include <iostream> // For debugging

#define ZERO_TOL	0.00001


/* Vector3d
   Three dimensional vector class made for doing geometric computations on points in
   3-space.  Contains most basic vector operations.
   ================================================================================== */
// Empty constructor [unit test in VectorCreation()]
Vector3d::Vector3d() { x = 0; y = 0; z = 0;}

// Basic constructor [unit test in VectorCreation()]
Vector3d::Vector3d(double x_, double y_, double z_)
{
	x = x_;
	y = y_;
	z = z_;
}

bool Vector3d::operator==(const Vector3d& other) const
{
	return this->DistanceTo(other) < ZERO_TOL;
}

// Vector addition operator [unit test in VectorOperators()]
Vector3d Vector3d::operator+(const Vector3d& other) const
{
	return Vector3d(x + other.x, y + other.y, z + other.z);
}

// Vector subtraction operator [unit test in VectorOperators()]
Vector3d Vector3d::operator-(const Vector3d& other) const
{
	return Vector3d(x - other.x, y - other.y, z - other.z);
}

// Vector scaling operator [unit test in VectorOperators()]
Vector3d Vector3d::operator*(const double& scale) const
{
	return Vector3d(x * scale, y * scale, z * scale);
}

// Vector rotation around x axis [unit test in VectorRotations()]
Vector3d Vector3d::RotateAboutX(double angle) const
{
	// Performs a basic vector rotation around the x axis using the math
	// found in http://www.cs.helsinki.fi/group/goa/mallinnus/3dtransf/3drot.html
	double nx = x;
	double ny = y * cos(angle) - z * sin(angle);
	double nz = y * sin(angle) + z * cos(angle);
	return Vector3d(nx, ny, nz);
}

// Vector rotation around y axis [unit test in VectorRotations()]
Vector3d Vector3d::RotateAboutY(double angle) const
{
	// Performs a basic vector rotation around the y axis using the math
	// found in http://www.cs.helsinki.fi/group/goa/mallinnus/3dtransf/3drot.html
	double nx = z * sin(angle) + x * cos(angle);
	double ny = y;
	double nz = z * cos(angle) - x * sin(angle);
	return Vector3d(nx, ny, nz);
}

// Vector rotation around z axis [unit test in VectorRotations()]
Vector3d Vector3d::RotateAboutZ(double angle) const
{
	// Performs a basic vector rotation around the z axis using the math
	// found in http://www.cs.helsinki.fi/group/goa/mallinnus/3dtransf/3drot.html
	double nx = x * cos(angle) - y * sin(angle);
	double ny = x * sin(angle) + y * cos(angle);
	double nz = z;
	return Vector3d(nx, ny, nz);
}

// Computes the length of the vector (distance to the origin) [unit test in VectorLengths()]
double Vector3d::Length() const
{
	return sqrt(x * x + y * y + z * z);
}

// Computes the dot product of this vector to another vector [unit test in VectorUnitAndDot()]
double Vector3d::Dot(const Vector3d& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

// Computes the distance between this vector and another vector [unit test in VectorLengths()]
double Vector3d::DistanceTo(const Vector3d& other) const
{
	return sqrt( pow(x - other.x, 2) + pow(y - other.y, 2) + pow(z - other.z, 2));
}

// Computes and returns a unit vector for the given vector [unit test in VectorUnitAndDot()]
Vector3d Vector3d::Unit() const
{
	double l = Length();
	return Vector3d(x / l, y / l, z / l);
}

double Vector3d::AngleTo(const Vector3d& other) const
{
	return AngleBetweenVectors(*this, other);
}

// Returns a string with the x, y, and z components of the vector assembled in a
// comma separated list.
std::string Vector3d::Text()
{
	std::stringstream s;
	s << x << ", " << y << ", " << z;
	return s.str();
}


// Computes the angle between two arbitrary vectors [unit test in VectorAngles()]
double AngleBetweenVectors(Vector3d v0, Vector3d v1)
{
	return acos(v0.Dot(v1) / (v0.Length() * v1.Length()));
}
