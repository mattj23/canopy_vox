#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <vector>
#include <tuple>
#include <string>
#include <sstream>

struct Vector3d
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

    // Operators
    Vector3d operator+(const Vector3d& other) const;
    Vector3d operator-(const Vector3d& other) const;
    Vector3d operator*(const double& scale) const;

    // Text
    std::string Text(void);
};

class Ray3d
{
public:
    Vector3d start;
    Vector3d direction;

    Ray3d();
    Ray3d(Vector3d start_, Vector3d direction_);
};

bool IsOnSegment(Vector3d start, Vector3d end, Vector3d test);
double GetAngle(Vector3d v1, Vector3d v2);  // REmove, this is a special case 2d function
double AngleBetweenVectors(Vector3d v0, Vector3d v1);   // Generalized 3d case
int ManifoldPreceedingPointIndex(Vector3d testPoint, const std::vector<Vector3d>& manifold);

// Projection functions
std::tuple<Vector3d, int> ClosestPoint2(Vector3d point, const std::vector<Vector3d>& manifold);
Vector3d ProjectOntoSegment(const Vector3d &p0, const Vector3d &p1, const Vector3d &point);
Vector3d ProjectOntoRay(const Ray3d &ray, const Vector3d &point);
std::tuple<bool, Vector3d> ComputeIntersection(Vector3d a, Vector3d b, Vector3d c, Vector3d d);
std::vector<Vector3d> GetIntersections(Vector3d a, Vector3d b, std::vector<Vector3d> manifold);

// Manifold splitting, sorting, and filtering functions
double GetManifoldLength(const std::vector<Vector3d> &manifold);
double GetLengthAlongManifold(const std::vector<Vector3d> &manifold, Vector3d point);

std::vector<Vector3d> ManifoldSplitBetween(Vector3d splitPoint1, Vector3d splitPoint2, const std::vector<Vector3d>& manifold);
std::vector<Vector3d> RemoveDuplicates(std::vector<Vector3d> manifold, double tolerance);

std::vector<Vector3d> FilterExtremesInX(std::vector<Vector3d> manifold, double lbound, double ubound);
std::vector<Vector3d> FilterExtremesInY(std::vector<Vector3d> manifold, double lbound, double ubound);
std::vector<Vector3d> FilterExtremesInZ(std::vector<Vector3d> manifold, double lbound, double ubound);
std::vector<Vector3d> FilterExtremesInR(const std::vector<Vector3d> &manifold, const Ray3d &axis, double lbound, double ubound);

std::vector<Vector3d> ArrangeByAngleAroundX(const std::vector<Vector3d> &manifold);
std::vector<Vector3d> ArrangeByAngleAroundY(const std::vector<Vector3d> &manifold);
std::vector<Vector3d> ArrangeByAngleAroundZ(const std::vector<Vector3d> &manifold);

std::vector<Vector3d> ArrangeClockwiseAroundZ(const std::vector<Vector3d> &manifold);
std::vector<Vector3d> ArrangeClockwiseAroundX(const std::vector<Vector3d> &manifold);

Vector3d GetHighestRadialPoint(const std::vector<Vector3d> &manifold, const Ray3d &axis);
Vector3d GetLowestRadialPoint(const std::vector<Vector3d> &manifold, const Ray3d &axis);

std::vector<Vector3d> ResampleCurve(const std::vector<Vector3d> &manifold, double samplingDistance);
Vector3d PointAlongManifold(const std::vector<Vector3d> &manifold, double distanceAlongCurve);


// misc
bool angleCompare(const std::tuple<double, Vector3d> &lhs, const std::tuple<double, Vector3d> &rhs);

#endif // VECTOR3D_H
