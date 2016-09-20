/*
	Vector3d Library
	Author: Matt Jarvis, 2013

	This vector library defines a struct that represents a vector in R3 space and
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


/* Vector3d struct
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
	return AngleBetweenVectors(&this, other);
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

// Checks if a test point is on the segment start->end [unit test in CheckIsOnSegment()]
bool IsOnSegment(Vector3d start, Vector3d end, Vector3d test)
{
	Vector3d p = ProjectOntoSegment(start, end, test);
	if (test.DistanceTo(p) > ZERO_TOL)
		return false;
	double segmentLength = start.DistanceTo(end);
	double d1 = start.DistanceTo(p);
	double d2 = end.DistanceTo(p);

	if (d1 > segmentLength || d2 > segmentLength)
		return false;
	return true;
}
