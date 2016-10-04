/*
	Vector3d Library
	Author: Matt Jarvis, 2013

	This vector library defines a struct that represents a vector in R3 space and
	defines basic operations relating to that type, including addition, scalar
	multiplication, dot products, distances, and lengths.

*/

#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <vector>
#include <tuple>
#include <string>
#include <sstream>

class Vector3d
{
public:
    double x;
    double y;
    double z;

    Vector3d();
    Vector3d(double x_, double y_, double z_);

    Vector3d RotateAboutX(double angle) const;
    Vector3d RotateAboutY(double angle) const;
    Vector3d RotateAboutZ(double angle) const;
    Vector3d Unit(void) const;

    double Dot(const Vector3d& other) const;
    double DistanceTo(const Vector3d& other) const;
    double Length(void) const;
    double AngleTo(const Vector3d& other) const;

    // Operators
    Vector3d operator+(const Vector3d& other) const;
    Vector3d operator-(const Vector3d& other) const;
    Vector3d operator*(const double& scale) const;
    bool operator==(const Vector3d& other) const;

    // Text
    std::string Text(void);
};

double AngleBetweenVectors(Vector3d v0, Vector3d v1);   // Generalized 3d case


#endif // VECTOR3D_H
