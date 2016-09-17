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


// Returns a string with the x, y, and z components of the vector assembled in a
// comma separated list.
std::string Vector3d::Text()
{
	std::stringstream s;
	s << x << ", " << y << ", " << z;
	return s.str();
}



/* Ray3d class
   Class which represents a ray in space, with a starting position and a direction
   vector.
   ================================================================================*/
Ray3d::Ray3d() {start = Vector3d(); direction = Vector3d(1,0,0);}

Ray3d::Ray3d(Vector3d start_, Vector3d direction_)
{
	start = start_;
	direction = direction_.Unit();
}


/* Misc functions */
// ==================================================================================
// Special 2D case cross product, should be removed
double SpecialCrossProduct(const Vector3d& v, const Vector3d& w)
{
	return v.z * w.y - v.y * w.z;
}

// Computes the angle between two arbitrary vectors [unit test in VectorAngles()]
double AngleBetweenVectors(Vector3d v0, Vector3d v1)
{
	return acos(v0.Dot(v1) / (v0.Length() * v1.Length()));
}

// Projects a point onto segment p0->p1 [unit test in VectorProjection()]
Vector3d ProjectOntoSegment(const Vector3d &p0, const Vector3d &p1, const Vector3d &point)
{
	Vector3d u = point - p0;
	Vector3d v = (p1 - p0).Unit();
	double s   = u.Dot(v);
	Vector3d r = v * s;
	return p0 + r;
}

// Projects a point onto ray [unit test in VectorProjection()]
Vector3d ProjectOntoRay(const Ray3d &ray, const Vector3d &point)
{
	// This should use ProjectOntoSegment
	Vector3d u = point - ray.start;
	Vector3d v = ray.direction;
	double   s = u.Dot(v);
	Vector3d r = v * s;
	return ray.start + r;
}

// Returns the length of a vector manifold [unit test in CheckManifoldLength()]
double GetManifoldLength(const std::vector<Vector3d> &manifold)
{
	double l = 0;
	for (int i = 0; i < manifold.size() - 1; ++i)
	{
		l += manifold[i].DistanceTo(manifold[i+1]);
	}
	return l;
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

// Gets the index of the preceeding point on the manifold [unit test in CheckManifoldPreceedingPointIndex()]
int ManifoldPreceedingPointIndex(Vector3d testPoint, const std::vector<Vector3d>& manifold)
{
	std::tuple<Vector3d, int> result = ClosestPoint2(testPoint, manifold);
	return std::get<1>(result);
}

// Splits a manifold between two points and returns the middle section [unit test in CheckManifoldSplit()]
std::vector<Vector3d> ManifoldSplitBetween(Vector3d splitPoint1, Vector3d splitPoint2, const std::vector<Vector3d>& manifold)
{
	int split1 = ManifoldPreceedingPointIndex(splitPoint1, manifold);
	int split2 = ManifoldPreceedingPointIndex(splitPoint2, manifold);
	std::vector<Vector3d> splitManifold;
	splitManifold.clear();

	int s1;
	int s2;
	Vector3d p1;
	Vector3d p2;

	if (split1 < split2)
	{
		s1 = split1;
		s2 = split2;
		p1 = splitPoint1;
		p2 = splitPoint2;
	}
	else
	{
		s1 = split2;
		s2 = split1;
		p1 = splitPoint2;
		p2 = splitPoint1;
	}

	splitManifold.push_back(p1);
	for (int i = 0; i < manifold.size(); i++)
	{
		if (i > (s1) && i <= s2)
		{
			splitManifold.push_back(manifold[i]);
		}
	}

	// if splitPoint2 is after the last point on the manifold (that is, past 90 degrees on a
	// comparison such that the projection onto the last segment is out of bounds and the
	// last point itself is returned as the preceeding point index), it is possible to have
	// the last point so close that it doubles up.  As a result it is necessary to check here
	// to make sure that we aren't repeating the final point.
	if (splitManifold.back().DistanceTo(splitPoint2) > ZERO_TOL)
		splitManifold.push_back(splitPoint2);
	return splitManifold;
}

// Locates the point on a manifold closest to the test point and returns a tuple with the
// closest point and the index of the manifold point which is just before it.
// [unit test in CheckClosestPoints()]
std::tuple<Vector3d, int> ClosestPoint2(Vector3d point, const std::vector<Vector3d>& manifold)
{

	std::vector<std::tuple<double, Vector3d, int>> rangeList;

	// Add the first point
	rangeList.push_back(std::make_tuple(point.DistanceTo(manifold[0]), manifold[0], 0));

	for (int i = 1; i < manifold.size(); i++)
	{
		Vector3d p0 = manifold[i-1];
		Vector3d p1 = manifold[i];

		// If the projection of point onto segment p0->p1 lies on the segment, we
		// add it to rangeList for checking as well

		Vector3d r    = ProjectOntoSegment(p0, p1, point);
		double length = p0.DistanceTo(p1);
		double l0     = p0.DistanceTo(r);
		double l1     = p1.DistanceTo(r);
		if (l0 < length && l1 < length)
			rangeList.push_back(std::make_tuple(point.DistanceTo(r), r, i-1));

		// Add point i
		rangeList.push_back(std::make_tuple(point.DistanceTo(p1), p1, i));
	}

	// Find the closest point
	int k = 0;
	double minDist = std::get<0>(rangeList[0]);
	for (int i = 1; i < rangeList.size(); i++)
	{
		if (std::get<0>(rangeList[i]) < minDist)
		{
			minDist = std::get<0>(rangeList[i]);
			k = i;
		}
	}

	return std::make_tuple(std::get<1>(rangeList[k]), std::get<2>(rangeList[k]));
}


// Computes the intersection between two segments, specified by the start and end points of each
// segment.  In the case of a perfect intersection, the point of intersection is returned and the
// first value of the return tuple takes the value "true".  In the case of a skew intersection, the
// first tuple value is again "true", but the returned Vector3d is the midpoint of the two points
// which represent the segment of closest approach between the two segments.  In the case that the
// intersection point lies off of the ends of the segment, the first tuple value returns false.
// [unit test in CheckIntersections()]
std::tuple<bool, Vector3d> ComputeIntersection(Vector3d ray1Start, Vector3d ray1End, Vector3d ray2Start, Vector3d ray2End)
{
	Vector3d P0 = ray1Start;
	Vector3d Q0 = ray2Start;
	Vector3d u = (ray1End - ray1Start).Unit();
	Vector3d v = (ray2End - ray2Start).Unit();

	Vector3d w0 = P0 - Q0;
	double a = u.Dot(u);
	double b = u.Dot(v);
	double c = v.Dot(v);
	double d = u.Dot(w0);
	double e = v.Dot(w0);
    double sc = (b * e - c * d) / (a * c - b * b);
    double tc = (a * e - b * d) / (a * c - b * b);

    Vector3d P = (u * sc) + P0;		// Projection of closest point onto ray1
    Vector3d Q = (v * tc) + Q0;		// Projection of closest point onto ray2

    // Both P and Q must lie on their respective vectors
    if (sc < 0)
    	return std::make_tuple(false, Vector3d());
    if (tc < 0)
    	return std::make_tuple(false, Vector3d());
    if (ray1Start.DistanceTo(P) > ray1Start.DistanceTo(ray1End))
    	return std::make_tuple(false, Vector3d());
    if (ray2Start.DistanceTo(Q) > ray2Start.DistanceTo(ray2End))
    	return std::make_tuple(false, Vector3d());

    Vector3d i = (P + Q) * 0.5;
    if (i.x != i.x || i.y != i.y || i.z != i.z)
    	return std::make_tuple(false, Vector3d());
    return std::make_tuple(true, (P + Q) * 0.5);
}


// Builds a list of intersections between a segment (a -> b) and a polyline manifold.  Uses
// ComputeIntersection.  [unit test in CheckIntersections()]
std::vector<Vector3d> GetIntersections(Vector3d a, Vector3d b, std::vector<Vector3d> manifold)
{
	std::vector<Vector3d> intersections;
	intersections.clear();

	for (int i = 0; i < manifold.size() - 1; i++)
	{
		Vector3d p0 = manifold[i];
		Vector3d p1 = manifold[i+1];
		std::tuple<bool, Vector3d> result = ComputeIntersection(a, b, p0, p1);

		if (std::get<0>(result) == true)
		{
			intersections.push_back(std::get<1>(result));
		}
	}
	return intersections;
}

// COMPREHENSIVELY removes duplicates within the manifold.  Will cause problems on manifolds
// which cross themselves.  [unit test in CheckRemoveAndResample()]
std::vector<Vector3d> RemoveDuplicates(std::vector<Vector3d> manifold, double tolerance)
{
	std::vector<Vector3d> cleaned;
	cleaned.clear();

	// Pop an element off the vector
	for (int i = 0; i < manifold.size(); ++i)
	{
		bool nonRepeating = true;
		for (int j = 0; j < cleaned.size(); ++j)
		{
			if (manifold[i].DistanceTo(cleaned[j]) <= tolerance)
			{
				nonRepeating = false;
				break;
			}
		}

		if (nonRepeating)
		{
			cleaned.push_back(manifold[i]);
		}
	}
	return cleaned;
}

// Resamples a curve with a given spacing. [unit test in CheckRemoveAndResample()]
std::vector<Vector3d> ResampleCurve(const std::vector<Vector3d> &manifold, double samplingDistance)
{
	double l = samplingDistance;
	double total = GetManifoldLength(manifold);

	// Add the first point
	std::vector<Vector3d> resampled;
	resampled.push_back(manifold[0]);
	while (l <= total)
	{
		resampled.push_back(PointAlongManifold(manifold, l));
		l += samplingDistance;
	}
	return resampled;
}

// Returns a point on a manifold a given distance from the first point
// [unit test in CheckPointsAlongManifold()]
Vector3d PointAlongManifold(const std::vector<Vector3d> &manifold, double distanceAlongCurve)
{
	double length = 0;

	// Check to make sure distance isn't longer than the actual
	if (distanceAlongCurve > GetManifoldLength(manifold))
		return manifold.back();

	for (int i = 0; i < manifold.size() - 1; ++i)
	{
		Vector3d v = manifold[i+1] - manifold[i];

		double distanceHere  = length;
		double distanceThere = length + v.Length();

		if (distanceAlongCurve >= distanceHere && distanceAlongCurve <= distanceThere)
		{
			Vector3d u = v.Unit();
			return manifold[i] + (u * (distanceAlongCurve - distanceHere));
		}
		length = distanceThere;
	}
	int last = manifold.size() - 1;
	Vector3d v = manifold[last] - manifold[last - 1];
	Vector3d u = v.Unit();
	return manifold[last - 1] + (u * (distanceAlongCurve - length));
}

// Finds the distance of the closest point on the manifold to the beginning of
// the manifold.  [unit test in CheckPointsAlongManifold()]
double GetLengthAlongManifold(const std::vector<Vector3d> &manifold, Vector3d point)
{
	std::tuple<Vector3d, int> result = ClosestPoint2(point, manifold);
	Vector3d closest = std::get<0>(result);
	int preceeding = std::get<1>(result);
	double totalLength = 0;

	// Adjust preceeding if necessary
	for (int i = 0; i < preceeding; ++i)
	{
		Vector3d v0 = manifold[i];
		Vector3d v1 = manifold[i+1];
		totalLength += v0.DistanceTo(v1);
	}

	return totalLength + manifold[preceeding].DistanceTo(closest);
}


// Should be removed
double GetAngle(Vector3d v1, Vector3d v2)
{
	Vector3d v1t = Vector3d(0, v1.y, v1.z);
	Vector3d v2t = Vector3d(0, v2.y, v2.z);
	return acos(v1t.Dot(v2t) / (v1t.Length() * v2t.Length()) );
}




// Returns a copy of the manifold with the elements lower than lbound and higher
// than ubound in the X direction removed.  [unit test in CheckFilteringExtremes()]
std::vector<Vector3d> FilterExtremesInX(std::vector<Vector3d> manifold, double lbound, double ubound)
{
	std::vector<Vector3d> cleaned;
	cleaned.clear();

	for (int i = 0; i < manifold.size(); ++i)
	{
		if (manifold[i].x >= lbound && manifold[i].x <= ubound)
			cleaned.push_back(manifold[i]);
	}

	return cleaned;
}

// Returns a copy of the manifold with the elements lower than lbound and higher
// than ubound in the Y direction removed.  [unit test in CheckFilteringExtremes()]
std::vector<Vector3d> FilterExtremesInY(std::vector<Vector3d> manifold, double lbound, double ubound)
{
	std::vector<Vector3d> cleaned;
	cleaned.clear();

	for (int i = 0; i < manifold.size(); ++i)
	{
		if (manifold[i].y >= lbound && manifold[i].y <= ubound)
			cleaned.push_back(manifold[i]);
	}

	return cleaned;
}

// Returns a copy of the manifold with the elements lower than lbound and higher
// than ubound in the Z direction removed.  [unit test in CheckFilteringExtremes()]
std::vector<Vector3d> FilterExtremesInZ(std::vector<Vector3d> manifold, double lbound, double ubound)
{
	std::vector<Vector3d> cleaned;
	cleaned.clear();

	for (int i = 0; i < manifold.size(); ++i)
	{
		if (manifold[i].z >= lbound && manifold[i].z <= ubound)
			cleaned.push_back(manifold[i]);
	}

	return cleaned;
}

// Returns a copy of the manifold with the radial extremes around the central axis
// removed from the vector. [unit test in CheckFilteringExtremes()]
std::vector<Vector3d> FilterExtremesInR(const std::vector<Vector3d> &manifold, const Ray3d &axis, double lbound, double ubound)
{
	std::vector<Vector3d> filtered;
	for (Vector3d v : manifold)
	{
		Vector3d p = ProjectOntoRay(axis, v);
		double   r = p.DistanceTo(v);

		if (r <= ubound && r >= lbound)
			filtered.push_back(v);
	}
	return filtered;
}

// returns the point with the highest distance from the given axis [unit test in CheckRadialExtremes()]
Vector3d GetHighestRadialPoint(const std::vector<Vector3d> &manifold, const Ray3d &axis)
{
	double rmax;
	Vector3d vmax;

	for (int i = 0; i < manifold.size(); ++i)
	{
		Vector3d p = ProjectOntoRay(axis, manifold[i]);
		double   r = p.DistanceTo(manifold[i]);
		if (i == 0 || rmax < r)
		{
			rmax = r;
			vmax = manifold[i];
		}
	}
	return vmax;
}

// returns the point with the lowest distance from the given axis [unit test in CheckRadialExtremes()]
Vector3d GetLowestRadialPoint(const std::vector<Vector3d> &manifold, const Ray3d &axis)
{
	double   rmin;
	Vector3d vmin;

	for (int i = 0; i < manifold.size(); ++i)
	{
		Vector3d p = ProjectOntoRay(axis, manifold[i]);
		double   r = p.DistanceTo(manifold[i]);
		if (i == 0 || rmin > r)
		{
			rmin = r;
			vmin = manifold[i];
		}
	}
	return vmin;
}

// Returns a copy of the manifold sorted by angle around the Z axis.  This is based on cmath's ATAN2
// function, which returns angles from -180 to +180. [unit test in CheckArrangeByAngle()]
std::vector<Vector3d> ArrangeByAngleAroundZ(const std::vector<Vector3d> &manifold)
{
    std::vector<Vector3d> assembled;
    std::vector<std::tuple<double, Vector3d>> sortAngles;
    for (Vector3d v : manifold)
    {
        double angle = atan2(v.y, v.x);
        sortAngles.push_back(std::make_tuple(angle, v));
    }
    std::sort(sortAngles.begin(), sortAngles.end(), angleCompare);
    for (std::tuple<double, Vector3d> t : sortAngles)
        assembled.push_back( std::get<1>(t) );
    return assembled;
}

// Returns a copy of the manifold sorted by angle around the Y axis.  This is based on cmath's ATAN2
// function, which returns angles from -180 to +180. [unit test in CheckArrangeByAngle()]
std::vector<Vector3d> ArrangeByAngleAroundY(const std::vector<Vector3d> &manifold)
{
    std::vector<Vector3d> assembled;
    std::vector<std::tuple<double, Vector3d>> sortAngles;
    for (Vector3d v : manifold)
    {
        double angle = atan2(v.z, v.x);
        sortAngles.push_back(std::make_tuple(angle, v));
    }
    std::sort(sortAngles.begin(), sortAngles.end(), angleCompare);
    for (std::tuple<double, Vector3d> t : sortAngles)
        assembled.push_back( std::get<1>(t) );
    return assembled;
}

// Returns a copy of the manifold sorted by angle around the X axis.  This is based on cmath's ATAN2
// function, which returns angles from -180 to +180. [unit test in CheckArrangeByAngle()]
std::vector<Vector3d> ArrangeByAngleAroundX(const std::vector<Vector3d> &manifold)
{
    std::vector<Vector3d> assembled;
    std::vector<std::tuple<double, Vector3d>> sortAngles;
    for (Vector3d v : manifold)
    {
        double angle = atan2(v.y, v.z);
        sortAngles.push_back(std::make_tuple(angle, v));
    }
    std::sort(sortAngles.begin(), sortAngles.end(), angleCompare);
    for (std::tuple<double, Vector3d> t : sortAngles)
        assembled.push_back( std::get<1>(t) );
    return assembled;
}


/*  Creates a copy of a Vector3d vector (manifold) to be arranged in the counterclockwise direction
    around the origin.  The function searches for the largest clockwise angular gap and then sets the
    clockwise side of it to be zero.  Then all items are sorted in terms of angle from there */
std::vector<Vector3d> ArrangeClockwiseAroundZ(const std::vector<Vector3d> &manifold)
{
    // Compute angles and sort them, from -180 to +180
    std::vector<std::tuple<double, Vector3d>> sortAngles;
    for (Vector3d v : manifold)
    {
        double angle = atan2(v.y, v.x);
        sortAngles.push_back(std::make_tuple(angle, v));
    }
    std::sort(sortAngles.begin(), sortAngles.end(), angleCompare);

    // Compute the CLOCKWISE gap for each element i to i-1.  If i==0, the gap is from i to
    // the last element in the list.  Thus element i is on the CLOCKWISE side of the recorded gap.
    std::vector<int> gaps;
    for (int i = 0; i < sortAngles.size(); ++i)
    {
        double theta0;
        double theta1 = std::get<0>(sortAngles[i])  * 180.0 / 3.14159;
        if (i == 0)
            theta0 = std::get<0>(sortAngles.back()) * 180.0 / 3.14159;
        else
            theta0 = std::get<0>(sortAngles[i-1]) * 180.0 / 3.14159;

        //std::cout<< i << ", " << theta1 << ", " << theta0 << ", span = ";
        double span;
        if (theta1 > theta0)
            span = theta1 - theta0;
        else
            span = theta1 - theta0 + 360.0;
        gaps.push_back(span);
    }

    // Now find the largest CLOCKWISE gap.  This element will be the first element in the rearranged
    // manifold.
    double maxIndex = 0;
    for (int i = 1; i < gaps.size(); ++i)
    {
        if (gaps[maxIndex] < gaps[i])
            maxIndex = i;
    }

    // Perform the vector copies from maxindex to the end of the sorted vector, then from the beginning
    // of the sorted vector to the maxindex
    std::vector<Vector3d> arrangedManifold;
    for (int i = maxIndex; i < sortAngles.size(); ++i)
        arrangedManifold.push_back( std::get<1>(sortAngles[i]) );
    for (int i = 0; i < maxIndex; ++i)
        arrangedManifold.push_back( std::get<1>(sortAngles[i]) );

    return arrangedManifold;
}

/*  Creates a copy of a Vector3d vector (manifold) to be arranged in the counterclockwise direction
    around the x-line.  The function searches for the largest clockwise angular gap and then sets the
    clockwise side of it to be zero.  Then all items are sorted in terms of angle from there */
std::vector<Vector3d> ArrangeClockwiseAroundX(const std::vector<Vector3d> &manifold)
{
    // Compute angles and sort them, from -180 to +180
    std::vector<std::tuple<double, Vector3d>> sortAngles;
    for (Vector3d v : manifold)
    {
        double angle = atan2(v.z, v.y);
        sortAngles.push_back(std::make_tuple(angle, v));
    }
    std::sort(sortAngles.begin(), sortAngles.end(), angleCompare);

    // Compute the CLOCKWISE gap for each element i to i-1.  If i==0, the gap is from i to
    // the last element in the list.  Thus element i is on the CLOCKWISE side of the recorded gap.
    std::vector<int> gaps;
    for (int i = 0; i < sortAngles.size(); ++i)
    {
        double theta0;
        double theta1 = std::get<0>(sortAngles[i])  * 180.0 / 3.14159;
        if (i == 0)
            theta0 = std::get<0>(sortAngles.back()) * 180.0 / 3.14159;
        else
            theta0 = std::get<0>(sortAngles[i-1]) * 180.0 / 3.14159;

        //std::cout<< i << ", " << theta1 << ", " << theta0 << ", span = ";
        double span;
        if (theta1 > theta0)
            span = theta1 - theta0;
        else
            span = theta1 - theta0 + 360.0;
        gaps.push_back(span);
    }

    // Now find the largest CLOCKWISE gap.  This element will be the first element in the rearranged
    // manifold.
    double maxIndex = 0;
    for (int i = 1; i < gaps.size(); ++i)
    {
        if (gaps[maxIndex] < gaps[i])
            maxIndex = i;
    }

    // Perform the vector copies from maxindex to the end of the sorted vector, then from the beginning
    // of the sorted vector to the maxindex
    std::vector<Vector3d> arrangedManifold;
    for (int i = maxIndex; i < sortAngles.size(); ++i)
        arrangedManifold.push_back( std::get<1>(sortAngles[i]) );
    for (int i = 0; i < maxIndex; ++i)
        arrangedManifold.push_back( std::get<1>(sortAngles[i]) );

    return arrangedManifold;
}









// Should be moved to vector3d.cpp
bool angleCompare(const std::tuple<double, Vector3d> &lhs, const std::tuple<double, Vector3d> &rhs)
{
    return std::get<0>(lhs) < std::get<0>(rhs);
}
