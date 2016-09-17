#include <cmath>
#include <vector>
#include <tuple>
#include <iostream>

#include "vector3d.h"

#define DEBUG       true
#define ZERO_TOL    0.000001
#define ZERO_TOL_2  0.00001

using namespace std;

vector<Vector3d> MakeTestManifold()
{
    vector<Vector3d> test;
    test.push_back(Vector3d(0, 0,-1));
    test.push_back(Vector3d(0, 0, 0));
    test.push_back(Vector3d(1, 0, 0));
    test.push_back(Vector3d(1, 1, 0));
    test.push_back(Vector3d(1, 1, 1));
    return test;
}

bool VectorCreation()
{
    bool passed = true;
    Vector3d empty;
    Vector3d v0(1.0, 1.0, 1.0);
    Vector3d v1 = Vector3d(2.0, 2.0, 2.0);

    if (empty.x != 0) passed = false;
    if (empty.y != 0) passed = false;
    if (empty.z != 0) passed = false;

    if (v0.x != 1.0) passed = false;    
    if (v0.y != 1.0) passed = false;        
    if (v0.z != 1.0) passed = false;        

    if (v1.x != 2.0) passed = false;    
    if (v1.y != 2.0) passed = false;        
    if (v1.z != 2.0) passed = false;        

    return passed;
}

bool VectorLengths()
{
    bool passed = true;
    Vector3d v0;
    Vector3d v1(100, 0, 0);
    Vector3d v2(0, 100, 0);
    Vector3d v3(0, 0, 100);
    Vector3d h1(50, 0, 0);
    Vector3d h2(0, 50, 0);
    Vector3d h3(0, 0, 50);

    if (v1.Length() != 100) passed = false;
    if (v2.Length() != 100) passed = false;
    if (v3.Length() != 100) passed = false;

    if (v1.DistanceTo(h1) != 50) passed = false;
    if (v2.DistanceTo(h2) != 50) passed = false;
    if (v3.DistanceTo(h3) != 50) passed = false;

    if (v1.DistanceTo(v1) != 0.0) passed = false;
    return passed;
}

bool VectorOperators()
{
    bool passed = true;
    Vector3d v0;
    Vector3d v1(1.0, 1.0, 1.0);
    Vector3d v2(0.5, 0.5, 0.5);
    Vector3d v3(1.5, 1.5, 1.5);
    Vector3d t1(1, 0, 0);
    Vector3d t2(0, 1, 0);
    Vector3d t3(0, 0, 1);

    Vector3d r1 = v1 + v2;
    if (r1.DistanceTo(v3) != 0) passed = false;

    Vector3d r2 = v3 - v2;
    if (r2.DistanceTo(v1) != 0) passed = false;

    Vector3d r3 = v2 * 3;
    if (r3.DistanceTo(v3) != 0) passed = false;

    Vector3d r4 = t1 + t2 + t3;
    if (r4.DistanceTo(v1) != 0) passed = false;

    return passed;

}

bool VectorUnitAndDot()
{
    bool passed = true;
    bool linedown = false;

    Vector3d t1(100, 0, 0);
    Vector3d t2(0, 100, 0);
    Vector3d t3(0, 0, 100);

    // Unit vector generation tests
    if (fabs( t1.Unit().DistanceTo(Vector3d(1, 0, 0)) ) > ZERO_TOL) 
    {
        passed = false;
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . checking unit vector of t1 (" << t1.Text() << ")" << endl;
        cout << "                           got (" << t1.Unit().Text() << ")" << endl;
    }    

    if (fabs( (t1 + t2).Unit().DistanceTo(Vector3d(1 / sqrt(2.0), 1 / sqrt(2.0), 0)) ) > ZERO_TOL)
    {
        passed = false; 
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . checking unit vector of t1 + t2 (" << (t1+t2).Text() << ")" << endl;
        cout << "                                got (" << (t1+t2).Unit().Text() << ")" << endl;
        cout << "                           expected (" << Vector3d(1 / sqrt(2.0), 1 / sqrt(2.0), 0).Text() << ")" << endl;

    }
    
    if (fabs( (t1 + t2 + t3).Unit().Length() - 1) > ZERO_TOL)
    {
        passed = false;
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . checking unit vector of t1 + t2 + t3 (" << (t1+t2+t3).Unit().Text() << ")" << endl;
        cout << "                                length = (" << (t1+t2+t3).Unit().Length() << ")" << endl;
    }
    
    // Dot product tests
    Vector3d v0(1, 2, 3);
    Vector3d v1(4, 5, 6);
    if (v0.Dot(v1) != 32) 
    {
        passed = false;
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . dot product of (" << v0.Text() << ") and (" << v1.Text() << ") = " << v0.Dot(v1) << endl;
        
    }

    return passed;
}

bool VectorRotations()
{
    // checking against http://www.usbe.cas.cz/people/lazar/celler/online_tools.php
    bool passed = true;
    Vector3d v(1, 1, 1);
    if (v.RotateAboutX(-4.712385).DistanceTo(Vector3d(1,-1,1)) >    ZERO_TOL_2) passed = false;
    if (v.RotateAboutY(-4.712385).DistanceTo(Vector3d(1,1,-1)) >    ZERO_TOL_2) passed = false;
    if (v.RotateAboutZ(-4.712385).DistanceTo(Vector3d(-1,1,1)) >    ZERO_TOL_2) passed = false;
    if (v.RotateAboutX(-3.141590).DistanceTo(Vector3d(1,-1,-1)) >   ZERO_TOL_2) passed = false;
    if (v.RotateAboutY(-3.141590).DistanceTo(Vector3d(-1,1,-1)) >   ZERO_TOL_2) passed = false;
    if (v.RotateAboutZ(-3.141590).DistanceTo(Vector3d(-1,-1,1)) >   ZERO_TOL_2) passed = false;
    if (v.RotateAboutX(-1.570795).DistanceTo(Vector3d(1,1,-1)) >    ZERO_TOL_2) passed = false;
    if (v.RotateAboutY(-1.570795).DistanceTo(Vector3d(-1,1,1)) >    ZERO_TOL_2) passed = false;
    if (v.RotateAboutZ(-1.570795).DistanceTo(Vector3d(1,-1,1)) >    ZERO_TOL_2) passed = false;
    if (v.RotateAboutX(0.000000).DistanceTo(Vector3d(1,1,1)) >      ZERO_TOL_2) passed = false;
    if (v.RotateAboutY(0.000000).DistanceTo(Vector3d(1,1,1)) >      ZERO_TOL_2) passed = false;
    if (v.RotateAboutZ(0.000000).DistanceTo(Vector3d(1,1,1)) >      ZERO_TOL_2) passed = false;
    if (v.RotateAboutX(1.570795).DistanceTo(Vector3d(1,-1,1)) >     ZERO_TOL_2) passed = false;
    if (v.RotateAboutY(1.570795).DistanceTo(Vector3d(1,1,-1)) >     ZERO_TOL_2) passed = false;
    if (v.RotateAboutZ(1.570795).DistanceTo(Vector3d(-1,1,1)) >     ZERO_TOL_2) passed = false;
    if (v.RotateAboutX(3.141590).DistanceTo(Vector3d(1,-1,-1)) >    ZERO_TOL_2) passed = false;
    if (v.RotateAboutY(3.141590).DistanceTo(Vector3d(-1,1,-1)) >    ZERO_TOL_2) passed = false;
    if (v.RotateAboutZ(3.141590).DistanceTo(Vector3d(-1,-1,1)) >    ZERO_TOL_2) passed = false;
    if (v.RotateAboutX(4.712385).DistanceTo(Vector3d(1,1,-1)) >     ZERO_TOL_2) passed = false;
    if (v.RotateAboutY(4.712385).DistanceTo(Vector3d(-1,1,1)) >     ZERO_TOL_2) passed = false;
    if (v.RotateAboutZ(4.712385).DistanceTo(Vector3d(1,-1,1)) >     ZERO_TOL_2) passed = false;
    if (v.RotateAboutX(6.283180).DistanceTo(Vector3d(1,1,1)) >      ZERO_TOL_2) passed = false;
    if (v.RotateAboutY(6.283180).DistanceTo(Vector3d(1,1,1)) >      ZERO_TOL_2) passed = false;
    if (v.RotateAboutZ(6.283180).DistanceTo(Vector3d(1,1,1)) >      ZERO_TOL_2) passed = false;
    return passed;

}

bool VectorAngles()
{
    // checking against http://www.usbe.cas.cz/people/lazar/celler/online_tools.php
    bool passed = true;
    if (fabs(AngleBetweenVectors(   Vector3d(0.25,-0.57,-0.91),    Vector3d(-0.18,-0.16,0.21 ) ) - 114.29 * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(-0.70,0.18,0.88),     Vector3d(-0.67,-0.16,0.82 ) ) - 17.7   * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(-0.69,0.04,0.48),     Vector3d(0.47,-0.99,-0.11)  ) - 116.72 * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(-0.97,0.00,0.03),     Vector3d(-0.31,-0.74,0.49 ) ) - 69.78  * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.87,-0.82,-0.17),    Vector3d(0.45,-0.79,0.20 )  ) - 26.58  * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.29,-0.82,-0.63),    Vector3d(0.02,-0.17,-0.55)  ) - 37.36  * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.78,-0.81,-0.50),    Vector3d(0.24,0.63,-0.16)   ) - 106.56 * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.36,0.83,-0.94),     Vector3d(-0.85,-0.45,0.37 ) ) - 139.83 * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(-0.32,0.31,-0.07),    Vector3d(0.13,-0.16,-0.58)  ) - 100.5  * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(-0.80,-0.86,0.27),    Vector3d(0.37,-0.30,0.44 )  ) - 84.07  * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(-0.46,-0.26,0.53),    Vector3d(0.83,-0.22,-0.78)  ) - 148.22 * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.48,-0.90,-0.91),    Vector3d(-0.74,0.31,0.68 )  ) - 150.64 * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.26,0.33,-0.01),     Vector3d(-0.84,0.97,0.49 )  ) - 80.35  * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.03,0.95,-0.16),     Vector3d(0.03,-0.47,-0.59)  ) - 118.86 * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.76,0.77,-0.87),     Vector3d(0.59,0.00,0.81 )   ) - 100.62 * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.41,0.80,-0.60),     Vector3d(-0.46,0.67,-0.41)  ) - 52.9   * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.26,-0.09,0.72),     Vector3d(0.12,0.50,-0.65)   ) - 138.96 * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(-0.54,0.14,0.88),     Vector3d(0.01,-0.72,0.00 )  ) - 98.14  * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.26,0.85,-0.01),     Vector3d(0.73,-0.72,-0.02)  ) - 117.58 * 3.14159 / 180.0)    > 0.001) passed = false;
    if (fabs(AngleBetweenVectors(   Vector3d(0.93,-0.39,-0.33),    Vector3d(-0.35,-0.64,-0.02) ) - 95.13  * 3.14159 / 180.0)    > 0.001) passed = false;
    return passed;
}

bool VectorProjection()
{
    bool passed = true;

    Vector3d s;
    Vector3d e(1, 0, 0);
    Vector3d p(0.5, -1, 0);
    Vector3d r(0.5, 0, 0);

    if (ProjectOntoSegment(s, e, Vector3d(0.5, -1, 0)).DistanceTo(r) > ZERO_TOL) passed = false;
    if (ProjectOntoSegment(s, e, Vector3d(0.5,  1, 0)).DistanceTo(r) > ZERO_TOL) passed = false;
    if (ProjectOntoSegment(s, e, Vector3d(0.5, -1, 1)).DistanceTo(r) > ZERO_TOL) passed = false;
    if (ProjectOntoSegment(s, e, Vector3d(0.5,  1, 1)).DistanceTo(r) > ZERO_TOL) passed = false;

    if (ProjectOntoRay(Ray3d(s, e), Vector3d(0.5, -1, 0)).DistanceTo(r) > ZERO_TOL) passed = false;
    if (ProjectOntoRay(Ray3d(s, e), Vector3d(0.5,  1, 0)).DistanceTo(r) > ZERO_TOL) passed = false;
    if (ProjectOntoRay(Ray3d(s, e), Vector3d(0.5, -1, 1)).DistanceTo(r) > ZERO_TOL) passed = false;
    if (ProjectOntoRay(Ray3d(s, e), Vector3d(0.5,  1, 1)).DistanceTo(r) > ZERO_TOL) passed = false;


    return passed;
}

bool CheckManifoldLength()
{
    bool passed = true;
    vector<Vector3d> manifold = MakeTestManifold();
    if (GetManifoldLength(manifold) != 4)
        passed = false;
    return passed;
}

bool CompareManifolds(vector<Vector3d> m1, vector<Vector3d> m2)
{
    if (m1.size() != m2.size())
        return false;
    for (int i = 0; i < m1.size(); ++i)
    {
        if (m1[i].DistanceTo(m2[i]) > 0.0001)
            return false;
    }
    return true;
}

bool CheckIsOnSegment()
{
    bool passed = true;
    Vector3d s(0, 0, 0);
    Vector3d e(1, 0, 0);

    if (IsOnSegment(s, e, Vector3d(-1, 0, 0)) != false) passed = false;
    if (IsOnSegment(s, e, Vector3d( 0, 0, 0)) != true) passed = false;
    if (IsOnSegment(s, e, Vector3d(.5, 0, 0)) != true) passed = false;
    if (IsOnSegment(s, e, Vector3d( 1, 0, 0)) != true) passed = false;
    if (IsOnSegment(s, e, Vector3d(1.1,0, 0)) != false) passed = false;
    if (IsOnSegment(s, e, Vector3d(.5,.1, 0)) != false) passed = false;

    if (DEBUG && !passed)
    {
        cout << endl;
        cout << "  . checking -1, 0, 0 : got " << IsOnSegment(s, e, Vector3d(-1, 0, 0)) << ", expecting " << false << endl;
        cout << "  . checking  0, 0, 0 : got " << IsOnSegment(s, e, Vector3d( 0, 0, 0)) << ", expecting " << true << endl;
        cout << "  . checking .5, 0, 0 : got " << IsOnSegment(s, e, Vector3d(.5, 0, 0)) << ", expecting " << true << endl;
        cout << "  . checking  1, 0, 0 : got " << IsOnSegment(s, e, Vector3d( 1, 0, 0)) << ", expecting " << true << endl;
        cout << "  . checking 1.1,0, 0 : got " << IsOnSegment(s, e, Vector3d(1.1,0, 0)) << ", expecting " << false << endl;
        cout << "  . checking .5,.1, 0 : got " << IsOnSegment(s, e, Vector3d(.5,.1, 0)) << ", expecting " << false << endl;
   
    }
    return passed;
}

bool CheckManifoldPreceedingPointIndex()
{
    bool passed = true;
    vector<Vector3d> manifold = MakeTestManifold();

    if (ManifoldPreceedingPointIndex(Vector3d(0, 0, -0.5), manifold) != 0) passed = false;
    if (ManifoldPreceedingPointIndex(Vector3d(0, 0, 0),    manifold) != 1) passed = false;
    if (ManifoldPreceedingPointIndex(Vector3d(0.7, 0, 0),  manifold) != 1) passed = false;
    if (ManifoldPreceedingPointIndex(Vector3d(1, .5, 0.1), manifold) != 2) passed = false;
    if (ManifoldPreceedingPointIndex(Vector3d(1, 1, 0.8),  manifold) != 3) passed = false;
    if (ManifoldPreceedingPointIndex(Vector3d(1, 1, 1.2),  manifold) != 4) passed = false;

    if (DEBUG && !passed)
    {
        cout << endl;
        cout << "  . checking 0,   0,  -0.5 : got " << ManifoldPreceedingPointIndex(Vector3d(0, 0, -0.5), manifold) << ", expected " << 0 << endl;
        cout << "  . checking 0,   0,   0   : got " << ManifoldPreceedingPointIndex(Vector3d(0, 0, 0),    manifold) << ", expected " << 1 << endl;
        cout << "  . checking 0.7, 0,   0   : got " << ManifoldPreceedingPointIndex(Vector3d(0.7, 0, 0),  manifold) << ", expected " << 1 << endl;
        cout << "  . checking 1,   0.5, 0.1 : got " << ManifoldPreceedingPointIndex(Vector3d(1, .5, 0.1), manifold) << ", expected " << 2 << endl;
        cout << "  . checking 1,   1,   0.8 : got " << ManifoldPreceedingPointIndex(Vector3d(1, 1, 0.8),  manifold) << ", expected " << 3 << endl;
        cout << "  . checking 1,   1,   1.2 : got " << ManifoldPreceedingPointIndex(Vector3d(1, 1, 1.2),  manifold) << ", expected " << 4 << endl;        
    }

    return passed;
}

string ManifoldText(vector<Vector3d> manifold)
{
    stringstream s;
    for (Vector3d v : manifold)
        s << "(" << v.Text() << ") ";
    return s.str();
}

bool CheckManifoldSplit()
{
    bool passed = true;
    vector<Vector3d> manifold = MakeTestManifold();

    vector<Vector3d> r1;
    r1.push_back(Vector3d(0,0,-1));
    r1.push_back(Vector3d(0,0,0));
    r1.push_back(Vector3d(1,0,0));
    r1.push_back(Vector3d(1,.5,0));

    vector<Vector3d> r2;
    r2.push_back(Vector3d(.5,0,0));
    r2.push_back(Vector3d(1,0,0));
    r2.push_back(Vector3d(1,.5,0));

    vector<Vector3d> r3;
    r3.push_back(Vector3d(1, .5, 0));
    r3.push_back(Vector3d(1, 1, 0));
    r3.push_back(Vector3d(1, 1, 1));

    vector<Vector3d> c1 = ManifoldSplitBetween(Vector3d(0,0,-1), Vector3d(1,0.5,0), manifold);
    vector<Vector3d> c2 = ManifoldSplitBetween(Vector3d(.5,0,0), Vector3d(1,0.5,0), manifold);
    vector<Vector3d> c3 = ManifoldSplitBetween(Vector3d(1,.5,0), Vector3d(1, 1, 1), manifold);

    if (!CompareManifolds(c1, r1)) passed = false;
    if (!CompareManifolds(c2, r2)) passed = false;
    if (!CompareManifolds(c3, r3)) passed = false;

    if (DEBUG && !passed)
    {
        cout << endl;

        cout << "  . check 1, got      : ";
        for (Vector3d v : c1)
            cout << "(" << v.Text() << ") ";
        cout << endl;
        cout << "  . check 1, expected : ";
        for (Vector3d v : r1)
            cout << "(" << v.Text() << ") ";
        cout << endl << endl;

        cout << "  . check 2, got      : ";
        for (Vector3d v : c2)
            cout << "(" << v.Text() << ") ";
        cout << endl;
        cout << "  . check 2, expected : ";
        for (Vector3d v : r2)
            cout << "(" << v.Text() << ") ";
        cout << endl << endl;
        
        cout << "  . check 3, got      : ";
        for (Vector3d v : c3)
            cout << "(" << v.Text() << ") ";
        cout << endl;
        cout << "  . check 3, expected : ";
        for (Vector3d v : r3)
            cout << "(" << v.Text() << ") ";
        cout << endl << endl;
        
    }
    return passed;
}

bool CheckClosestPoints()
{
    bool passed = true;
    vector<Vector3d> manifold = MakeTestManifold();
    if ( get<1>( ClosestPoint2(Vector3d(0, 0, -0.5), manifold)) != 0) passed = false;
    if ( get<1>( ClosestPoint2(Vector3d(0, 0, 0),    manifold)) != 1) passed = false;
    if ( get<1>( ClosestPoint2(Vector3d(0.7, 0, 0),  manifold)) != 1) passed = false;
    if ( get<1>( ClosestPoint2(Vector3d(1, .5, 0.1), manifold)) != 2) passed = false;
    if ( get<1>( ClosestPoint2(Vector3d(1, 1, 0.8),  manifold)) != 3) passed = false;
    if ( get<1>( ClosestPoint2(Vector3d(1, 1, 1.2),  manifold)) != 4) passed = false;

    if (DEBUG && !passed)
    {
        cout << endl;
        cout << "  . index checking: " << endl;
        cout << "  . checking 0,   0,  -0.5 : got " << get<1>( ClosestPoint2(Vector3d(0, 0, -0.5), manifold) ) << ", expected " << 0 << endl;
        cout << "  . checking 0,   0,   0   : got " << get<1>( ClosestPoint2(Vector3d(0, 0, 0),    manifold) ) << ", expected " << 1 << endl;
        cout << "  . checking 0.7, 0,   0   : got " << get<1>( ClosestPoint2(Vector3d(0.7, 0, 0),  manifold) ) << ", expected " << 1 << endl;
        cout << "  . checking 1,   0.5, 0.1 : got " << get<1>( ClosestPoint2(Vector3d(1, .5, 0.1), manifold) ) << ", expected " << 2 << endl;
        cout << "  . checking 1,   1,   0.8 : got " << get<1>( ClosestPoint2(Vector3d(1, 1, 0.8),  manifold) ) << ", expected " << 3 << endl;
        cout << "  . checking 1,   1,   1.2 : got " << get<1>( ClosestPoint2(Vector3d(1, 1, 1.2),  manifold) ) << ", expected " << 4 << endl;        
    
    }

    if (get<0>(ClosestPoint2(Vector3d( 0, 1,-1.1), manifold)).DistanceTo(Vector3d( 0,  0, -1)) > 0.0001) passed = false;
    if (get<0>(ClosestPoint2(Vector3d(-.1, 0, .1), manifold)).DistanceTo(Vector3d( 0,  0,  0)) > 0.0001) passed = false;
    if (get<0>(ClosestPoint2(Vector3d(.5,  0, .2), manifold)).DistanceTo(Vector3d(.5,  0,  0)) > 0.0001) passed = false;
    if (get<0>(ClosestPoint2(Vector3d(1.2, 0,  0), manifold)).DistanceTo(Vector3d( 1,  0,  0)) > 0.0001) passed = false;
    if (get<0>(ClosestPoint2(Vector3d(1.2,.1, .1), manifold)).DistanceTo(Vector3d( 1, .1,  0)) > 0.0001) passed = false;
    if (get<0>(ClosestPoint2(Vector3d( 1,  1,1.2), manifold)).DistanceTo(Vector3d( 1,  1,  1)) > 0.0001) passed = false;

    if (DEBUG && !passed)
    {
        cout << endl;
        cout << "  . closest point checking:" << endl;
        cout << "  . checking Vector( 0, 1,-1.1) : got (" << get<0>(ClosestPoint2(Vector3d( 0, 1,-1.1), manifold)).Text() << "), expected ( 0,  0, -1)" << endl; 
        cout << "  . checking Vector(-.1, 0, .1) : got (" << get<0>(ClosestPoint2(Vector3d(-.1, 0, .1), manifold)).Text() << "), expected ( 0,  0,  0)" << endl; 
        cout << "  . checking Vector(.5,  0, .2) : got (" << get<0>(ClosestPoint2(Vector3d(.5,  0, .2), manifold)).Text() << "), expected (.5,  0,  0)" << endl; 
        cout << "  . checking Vector(1.2, 0,  0) : got (" << get<0>(ClosestPoint2(Vector3d(1.2, 0,  0), manifold)).Text() << "), expected ( 1,  0,  0)" << endl; 
        cout << "  . checking Vector(1.2,.1, .1) : got (" << get<0>(ClosestPoint2(Vector3d(1.2,.1, .1), manifold)).Text() << "), expected ( 1, .1,  0)" << endl; 
        cout << "  . checking Vector( 1,  1,1.2) : got (" << get<0>(ClosestPoint2(Vector3d( 1,  1,1.2), manifold)).Text() << "), expected ( 1,  1,  1)" << endl; 
    }

    return passed;
}

bool CheckIntersections()
{
    bool passed = true;
    Vector3d a1(0, 0, 0);
    Vector3d a2(1, 0, 0);

    Vector3d b1(0.5, -1, 0);
    Vector3d b2(0.5,  1, 0);

    // Basic intersection
    std::tuple<bool, Vector3d> r1 = ComputeIntersection(a1, a2, b1, b2);
    bool linedown = false;
    if (std::get<0>(r1) != true) 
    {
        passed = false;
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . basic intersection, expected true got false" << endl;
    }
    if (std::get<1>(r1).DistanceTo(Vector3d(0.5, 0, 0)) > 0.0001) 
    {
        passed = false;
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . basic intersection, expected (0.5, 0, 0), got " << get<1>(r1).Text();
    }

    // Skew intersection
    b1 = Vector3d(0.5, -1, 1);
    b2 = Vector3d(0.5,  1, 1);
    r1 = ComputeIntersection(a1, a2, b1, b2);
        if (std::get<0>(r1) != true) 
    {
        passed = false;
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . skew intersection, expected true got false" << endl;
    }
    if (std::get<1>(r1).DistanceTo(Vector3d(0.5, 0, 0.5)) > 0.0001) 
    {
        passed = false;
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . skew intersection, expected (0.5, 0, 0.5), got " << get<1>(r1).Text() << endl;
    }

    // Non intersection
    b1 = Vector3d(2, -1, 0);
    b2 = Vector3d(2,  1, 0);
    r1 = ComputeIntersection(a1, a2, b1, b2);
    if (std::get<0>(r1) != false)
    {
        passed = false;
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . non-intersection exected false, got true" << endl;
        cout << "  . non-intersection, got " << get<1>(r1).Text() << endl;
    }

    // Check GetIntersections
    vector<Vector3d> manifold;
    vector<Vector3d> check;
    manifold.push_back(Vector3d(0, 0, 0));
    manifold.push_back(Vector3d(1, 1, 0));
    manifold.push_back(Vector3d(2, 0, 0));
    manifold.push_back(Vector3d(3, 1, 0));
    manifold.push_back(Vector3d(4, 0, 0));

    check.push_back(Vector3d(0.5, 0.5, 0));
    // Partial check
    b1 = Vector3d(0, 0.5, 0);
    b2 = Vector3d(1, 0.5, 0);
    vector<Vector3d> result = GetIntersections(b1, b2, manifold);
    if (!CompareManifolds(result, check))
    {
        passed = false;
        if (DEBUG)
        {
            if (!linedown) {linedown = true; cout << endl;}
            cout << "  . get intersections, expected " << ManifoldText(check) << endl;
            cout << "                       got      " << ManifoldText(result) << endl;
        }
    }

    // Full check
    b1 = Vector3d(0, 0.5, 0);
    b2 = Vector3d(4, 0.5, 0);
    check.push_back(Vector3d(1.5, 0.5, 0));
    check.push_back(Vector3d(2.5, 0.5, 0));
    check.push_back(Vector3d(3.5, 0.5, 0));
    result = GetIntersections(b1, b2, manifold);
    if (!CompareManifolds(result, check))
    {
        passed = false;
        if (DEBUG)
        {
            if (!linedown) {linedown = true; cout << endl;}
            cout << "  . get intersections, expected " << ManifoldText(check) << endl;
            cout << "                       got      " << ManifoldText(result) << endl;
        }
    }

    // Empty check
    b1 = Vector3d(-1,  -1, 0);
    b2 = Vector3d(-1,  1, 0);
    result = GetIntersections(b1, b2, manifold);
    if (!result.empty())
    {
        passed = false;
        if (DEBUG)
        {
            if (!linedown) {linedown = true; cout << endl;}
            cout << "  . expected empty set, got " << ManifoldText(result) << endl;
        }
    }

    return passed;
}

bool CheckRemoveAndResample()
{
    bool passed = true;
    bool linedown = false;

    vector<Vector3d> check;
    check.push_back(Vector3d(0, 0, 0));
    check.push_back(Vector3d(1, 0, 0));
    check.push_back(Vector3d(1, 1, 0));
    check.push_back(Vector3d(2, 1, 2));
    check.push_back(Vector3d(2, 3, 0));
    check.push_back(Vector3d(0, 3, 2));
    check.push_back(Vector3d(0, 1, 0));

    vector<Vector3d> test;
    test.clear();
    test.push_back(Vector3d(0, 0, 0));
    test.push_back(Vector3d(1, 0, 0));
    test.push_back(Vector3d(1, 1, 0));
    test.push_back(Vector3d(2, 1, 2));
    test.push_back(Vector3d(2, 3, 0));
    test.push_back(Vector3d(0, 3, 2));
    test.push_back(Vector3d(0, 1, 0));
    
    // Check identical... make sure it doesn't change anything
    vector<Vector3d> result = RemoveDuplicates(test, 0);
    if (!CompareManifolds(check, result))
    {
        passed = false;
        if (DEBUG)
        {
            if (!linedown) {linedown = true; cout << endl;}
            cout << "  . remove none, expected " << ManifoldText(check) << endl;
            cout << "                 got      " << ManifoldText(result) << endl;
        }
    }

    // Add final duplicate with zero tolerance, make sure it removes it
    test.push_back(Vector3d(0, 1, 0));
    result = RemoveDuplicates(test, 0);
    if (!CompareManifolds(check, result))
    {
        passed = false;
        if (DEBUG)
        {
            if (!linedown) {linedown = true; cout << endl;}
            cout << "  . remove final duplicate, expected " << ManifoldText(check) << endl;
            cout << "                            got      " << ManifoldText(result) << endl;
        }
    }

    // Add middle, nonsequential duplicate
    test.clear();
    test.push_back(Vector3d(0, 0, 0));
    test.push_back(Vector3d(1, 0, 0));
    test.push_back(Vector3d(1, 1, 0));
    test.push_back(Vector3d(2, 1, 2));
    test.push_back(Vector3d(0, 0, 0));
    test.push_back(Vector3d(2, 3, 0));
    test.push_back(Vector3d(0, 3, 2));
    test.push_back(Vector3d(0, 1, 0));
    result = RemoveDuplicates(test, 0);
    if (!CompareManifolds(check, result))
    {
        passed = false;
        if (DEBUG)
        {
            if (!linedown) {linedown = true; cout << endl;}
            cout << "  . remove middle nonsequential duplicate, expected " << ManifoldText(check) << endl;
            cout << "                                           got      " << ManifoldText(result) << endl;
        }
    }

    // check larger tolerance
    test.push_back(Vector3d(1.1, 0, 0));
    test.push_back(Vector3d(2, 3.25, 0));
    result = RemoveDuplicates(test, 0.25);
    if (!CompareManifolds(check, result))
    {
        passed = false;
        if (DEBUG)
        {
            if (!linedown) {linedown = true; cout << endl;}
            cout << "  . remove with tolerance, expected " << ManifoldText(check) << endl;
            cout << "                           got      " << ManifoldText(result) << endl;
        }
    }

    // Check curve resampling
    test.clear();
    test.push_back(Vector3d(0, 0, 0));
    test.push_back(Vector3d(1, 0, 0));
    test.push_back(Vector3d(1, 1, 0));

    // Simple resampling, 0.5 interval
    check.clear();
    check.push_back(Vector3d(0, 0, 0));
    check.push_back(Vector3d(0.5, 0, 0));
    check.push_back(Vector3d(1, 0, 0));
    check.push_back(Vector3d(1, 0.5, 0));
    check.push_back(Vector3d(1, 1, 0));

    result = ResampleCurve(test, 0.5);
    if (!CompareManifolds(check, result))
    {
        passed = false;
        if (DEBUG)
        {
            if (!linedown) {linedown = true; cout << endl;}
            cout << "  . simple resampling, expected " << ManifoldText(check) << endl;
            cout << "                       got      " << ManifoldText(result) << endl;
        }
    }

    // Complex resampling, 0.3 interval
    check.clear();
    check.push_back(Vector3d(0, 0, 0));
    check.push_back(Vector3d(0.3, 0, 0));
    check.push_back(Vector3d(0.6, 0, 0));
    check.push_back(Vector3d(0.9, 0, 0));

    check.push_back(Vector3d(1, 0.2, 0));
    check.push_back(Vector3d(1, 0.5, 0));
    check.push_back(Vector3d(1, 0.8, 0));
    result = ResampleCurve(test, 0.3);
    if (!CompareManifolds(check, result))
    {
        passed = false;
        if (DEBUG)
        {
            if (!linedown) {linedown = true; cout << endl;}
            cout << "  . complex resampling, expected " << ManifoldText(check) << endl;
            cout << "                        got      " << ManifoldText(result) << endl;
        }
    }

    return passed;
}

bool CheckPointsAlongManifold()
{
    bool passed = true;
    bool linedown = false;
    vector<Vector3d> test = MakeTestManifold();

    if (Vector3d(0,0,-1 ).DistanceTo(PointAlongManifold(test, 0)) >     ZERO_TOL) passed = false;
    if (Vector3d(0,0,-.5).DistanceTo(PointAlongManifold(test, .5)) >    ZERO_TOL) passed = false;
    if (Vector3d(0,0,0  ).DistanceTo(PointAlongManifold(test, 1)) >     ZERO_TOL) passed = false;
    if (Vector3d(.5,0,0 ).DistanceTo(PointAlongManifold(test, 1.5)) >   ZERO_TOL) passed = false;
    if (Vector3d(1,.2,0 ).DistanceTo(PointAlongManifold(test, 2.2)) >   ZERO_TOL) passed = false;
    if (Vector3d(1,1,1).DistanceTo(PointAlongManifold(test, 4)) >       ZERO_TOL) passed = false;
    if (Vector3d(1,1,1).DistanceTo(PointAlongManifold(test, 5)) >       ZERO_TOL) passed = false;

    if (fabs(GetLengthAlongManifold(test, Vector3d(0,0,-.5)) - .5 ) > ZERO_TOL ) {if (!linedown) {linedown = true; cout << endl;} cout << "  . length along manifold (0,0,-.5) : expected .5  , got " << GetLengthAlongManifold(test, Vector3d(0,0,-.5)) << " (preceeding = " << ManifoldPreceedingPointIndex(Vector3d(0,0,-.5), test) << ")" << endl; passed = false;}
    if (fabs(GetLengthAlongManifold(test, Vector3d(0,0,0)  ) - 1  ) > ZERO_TOL ) {if (!linedown) {linedown = true; cout << endl;} cout << "  . length along manifold (0,0,0)   : expected 1   , got " << GetLengthAlongManifold(test, Vector3d(0,0,0)  ) << " (preceeding = " << ManifoldPreceedingPointIndex(Vector3d(0,0,0)  , test) << ")" << endl; passed = false;}
    if (fabs(GetLengthAlongManifold(test, Vector3d(0,0,-1) ) - 0  ) > ZERO_TOL ) {if (!linedown) {linedown = true; cout << endl;} cout << "  . length along manifold (0,0,-1)  : expected 0   , got " << GetLengthAlongManifold(test, Vector3d(0,0,-1) ) << " (preceeding = " << ManifoldPreceedingPointIndex(Vector3d(0,0,-1) , test) << ")" << endl; passed = false;}
    if (fabs(GetLengthAlongManifold(test, Vector3d(.5,0,0) ) - 1.5) > ZERO_TOL ) {if (!linedown) {linedown = true; cout << endl;} cout << "  . length along manifold (.5,0,0)  : expected 1.5 , got " << GetLengthAlongManifold(test, Vector3d(.5,0,0) ) << " (preceeding = " << ManifoldPreceedingPointIndex(Vector3d(.5,0,0) , test) << ")" << endl; passed = false;}
    if (fabs(GetLengthAlongManifold(test, Vector3d(1,.2,0) ) - 2.2) > ZERO_TOL ) {if (!linedown) {linedown = true; cout << endl;} cout << "  . length along manifold (1,.2,0)  : expected 2.2 , got " << GetLengthAlongManifold(test, Vector3d(1,.2,0) ) << " (preceeding = " << ManifoldPreceedingPointIndex(Vector3d(1,.2,0) , test) << ")" << endl; passed = false;}
    if (fabs(GetLengthAlongManifold(test, Vector3d(1,1,1)  ) - 4  ) > ZERO_TOL ) {if (!linedown) {linedown = true; cout << endl;} cout << "  . length along manifold (1,1,1)   : expected 4   , got " << GetLengthAlongManifold(test, Vector3d(1,1,1)  ) << " (preceeding = " << ManifoldPreceedingPointIndex(Vector3d(1,1,1)  , test) << ")" << endl; passed = false;}

    return passed;
}

bool CheckFilteringExtremes()
{
    bool passed = true;
    bool linedown = false;

    vector<Vector3d> check;
    vector<Vector3d> result;
    vector<Vector3d> test;
    test.push_back(Vector3d( 0,  9, 10));
    test.push_back(Vector3d( 1,  8, 11));
    test.push_back(Vector3d( 2,  7, 12));
    test.push_back(Vector3d( 3,  6, 13));
    test.push_back(Vector3d( 4,  5, 14));
    test.push_back(Vector3d( 5,  4, 15));
    test.push_back(Vector3d( 6,  3, 16));
    test.push_back(Vector3d( 7,  2, 17));
    test.push_back(Vector3d( 8,  1, 18));

    check.clear();
    check.push_back(Vector3d( 0,  9, 10));
    check.push_back(Vector3d( 1,  8, 11));
    check.push_back(Vector3d( 2,  7, 12));
    check.push_back(Vector3d( 3,  6, 13));
    check.push_back(Vector3d( 4,  5, 14));
    check.push_back(Vector3d( 5,  4, 15));
    check.push_back(Vector3d( 6,  3, 16));
    check.push_back(Vector3d( 7,  2, 17));
    check.push_back(Vector3d( 8,  1, 18));

    // Change nothing
    if (!CompareManifolds(FilterExtremesInX(test, -100, 100), check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering x, no removals" << endl;
        passed = false;
    }
    if (!CompareManifolds(FilterExtremesInY(test, -100, 100), check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering y, no removals" << endl;
        passed = false;
    }
    if (!CompareManifolds(FilterExtremesInZ(test, -100, 100), check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering z, no removals" << endl;
        passed = false;
    }

    // Remove everything
    if (!FilterExtremesInX(test, -100, -100).empty())
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering x, all removals" << endl;
        passed = false;
    }
    if (!FilterExtremesInY(test, -100, -100).empty())
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering y, all removals" << endl;
        passed = false;
    }
    if (!FilterExtremesInZ(test, -100, -100).empty())
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering z, all removals" << endl;
        passed = false;
    }

    check.clear();
    check.push_back(Vector3d( 2,  7, 12));
    check.push_back(Vector3d( 3,  6, 13));
    check.push_back(Vector3d( 4,  5, 14));
    check.push_back(Vector3d( 5,  4, 15));
    result = FilterExtremesInX(test, 2, 5);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering x, 2 to 5, got " << ManifoldText(result) << endl;
        passed = false;
    }

    check.clear();
    check.push_back(Vector3d( 4,  5, 14));
    check.push_back(Vector3d( 5,  4, 15));
    check.push_back(Vector3d( 6,  3, 16));
    check.push_back(Vector3d( 7,  2, 17));
    result = FilterExtremesInY(test, 2, 5);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering y, 2 to 5, got " << ManifoldText(result) << endl;
        passed = false;
    }

    check.clear();
    check.push_back(Vector3d( 2,  7, 12));
    check.push_back(Vector3d( 3,  6, 13));
    check.push_back(Vector3d( 4,  5, 14));
    check.push_back(Vector3d( 5,  4, 15));
    result = FilterExtremesInZ(test, 12, 15);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering z, 12 to 15, got " << ManifoldText(result) << endl;
        passed = false;
    }

    // Check radial filtering
    Ray3d zAxis(Vector3d(0, 0, 0), Vector3d(0, 0, 1));

    // all
    result = FilterExtremesInR(test, zAxis, 0, 100);
    if (!CompareManifolds(result, test))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering r, all, got " << ManifoldText(result) << endl;
        passed = false;
    }

    // none
    result = FilterExtremesInR(test, zAxis, 99, 100);
    if (!result.empty())
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering r, none expected, got " << ManifoldText(result) << endl;
        passed = false;
    }

    check.clear();
    check.push_back(Vector3d(3, 6, 13));     // rz = 6.708
    check.push_back(Vector3d(4, 5, 14));     // rz = 6.403
    check.push_back(Vector3d(5, 4, 15));     // rz = 6.403
    check.push_back(Vector3d(6, 3, 16));     // rz = 6.708
    result = FilterExtremesInR(test, zAxis, 6, 7);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . filtering r, 6 to 7, expected " << ManifoldText(check) << endl;
        cout << "                         got      " << ManifoldText(result) << endl;
        cout << "                         test     " << ManifoldText(test) << endl;

        passed = false;
    }

    return passed;
}

bool CheckRadialExtremes()
{
    bool passed = true;
    bool linedown = false;

    vector<Vector3d> test;
    test.push_back(Vector3d(0, 9, 10));     // rz = 9.000
    test.push_back(Vector3d(1, 8, 11));     // rz = 8.062
    test.push_back(Vector3d(2, 7, 12));     // rz = 7.280
    test.push_back(Vector3d(3, 6, 13));     // rz = 6.708
    test.push_back(Vector3d(4, 5, 14));     // rz = 6.403
    test.push_back(Vector3d(3, 4, 15));     // rz = ??
    test.push_back(Vector3d(6, 3, 16));     // rz = 6.708
    test.push_back(Vector3d(7, 2, 17));     // rz = 7.280
    test.push_back(Vector3d(8, 1, 18));     // rz = 8.062
    Ray3d zAxis(Vector3d(0, 0, 0), Vector3d(0, 0, 1));

    if (Vector3d(0, 9, 10).DistanceTo(GetHighestRadialPoint(test, zAxis)) > ZERO_TOL)
    {
        passed = false;
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . failed highest radial point" << endl;
    }
    if (Vector3d(3, 4, 15).DistanceTo(GetLowestRadialPoint(test, zAxis)) > ZERO_TOL)
    {
        passed = false;
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . failed highest radial point" << endl;
    }

    return passed;
}

bool CheckArrangeByAngle()
{
    bool passed = true;
    bool linedown = false;

    vector<Vector3d> test;
    vector<Vector3d> check;
    vector<Vector3d> result;

    // Check around RZ
    test.clear();
    test.push_back(Vector3d(-0.46,  0.80, -0.50));  // a = 119.90
    test.push_back(Vector3d(-0.92,  0.82, -0.62));  // a = 138.29
    test.push_back(Vector3d( 0.03,  0.48,  0.61));  // a = 86.42
    test.push_back(Vector3d( 0.07,  0.29,  0.46));  // a = 76.43
    test.push_back(Vector3d( 0.68, -0.68,  0.86));  // a = -45.00
    check.clear();
    check.push_back(Vector3d( 0.68, -0.68,  0.86));  // a = -45.00
    check.push_back(Vector3d( 0.07,  0.29,  0.46));  // a = 76.43
    check.push_back(Vector3d( 0.03,  0.48,  0.61));  // a = 86.42
    check.push_back(Vector3d(-0.46,  0.80, -0.50));  // a = 119.90
    check.push_back(Vector3d(-0.92,  0.82, -0.62));  // a = 138.29
    result = ArrangeByAngleAroundZ(test);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . arranging around z, expected " << ManifoldText(check) << endl;
        cout << "                        got      " << ManifoldText(result) << endl;
        passed = false;
    }

    test.clear();
    test.push_back(Vector3d(-0.03,  0.97,  1.00));  // a = 91.77
    test.push_back(Vector3d(-0.25, -0.29, -0.42));  // a = -130.76
    test.push_back(Vector3d(-0.15,  0.31, -0.50));  // a = 115.82
    test.push_back(Vector3d( 0.06, -0.59,  0.82));  // a = -84.19
    test.push_back(Vector3d(-0.02, -0.99,  0.18));  // a = -91.16
    check.clear();
    check.push_back(Vector3d(-0.25, -0.29, -0.42));  // a = -130.76
    check.push_back(Vector3d(-0.02, -0.99,  0.18));  // a = -91.16
    check.push_back(Vector3d( 0.06, -0.59,  0.82));  // a = -84.19
    check.push_back(Vector3d(-0.03,  0.97,  1.00));  // a = 91.77
    check.push_back(Vector3d(-0.15,  0.31, -0.50));  // a = 115.82
    result = ArrangeByAngleAroundZ(test);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . arranging around z, expected " << ManifoldText(check) << endl;
        cout << "                        got      " << ManifoldText(result) << endl;
        passed = false;
    }

    test.clear();
    test.push_back(Vector3d(-0.57, -0.22,  0.53));  // a = -158.90
    test.push_back(Vector3d( 0.02, -0.05,  0.30));  // a = -68.20
    test.push_back(Vector3d(-0.86, -0.86,  0.75));  // a = -135.00
    test.push_back(Vector3d(-0.45,  0.26, -0.25));  // a = 149.98
    test.push_back(Vector3d(-0.47,  0.95, -0.72));  // a = 116.32
    check.clear();
    check.push_back(Vector3d(-0.57, -0.22,  0.53));  // a = -158.90
    check.push_back(Vector3d(-0.86, -0.86,  0.75));  // a = -135.00
    check.push_back(Vector3d( 0.02, -0.05,  0.30));  // a = -68.20
    check.push_back(Vector3d(-0.47,  0.95, -0.72));  // a = 116.32
    check.push_back(Vector3d(-0.45,  0.26, -0.25));  // a = 149.98
    result = ArrangeByAngleAroundZ(test);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . arranging around z, expected " << ManifoldText(check) << endl;
        cout << "                        got      " << ManifoldText(result) << endl;
        passed = false;
    }

    // CHECK AROUND Y
    test.clear();
    test.push_back(Vector3d( 0.57, -0.63, -0.55));  // a = -43.98
    test.push_back(Vector3d( 0.91,  0.20, -0.91));  // a = -45.00
    test.push_back(Vector3d( 0.45, -0.34,  0.72));  // a = 57.99
    test.push_back(Vector3d( 0.95,  0.79,  0.09));  // a =  5.41
    test.push_back(Vector3d( 0.14,  0.31,  0.79));  // a = 79.95
    check.clear();
    check.push_back(Vector3d( 0.91,  0.20, -0.91));  // a = -45.00
    check.push_back(Vector3d( 0.57, -0.63, -0.55));  // a = -43.98
    check.push_back(Vector3d( 0.95,  0.79,  0.09));  // a =  5.41
    check.push_back(Vector3d( 0.45, -0.34,  0.72));  // a = 57.99
    check.push_back(Vector3d( 0.14,  0.31,  0.79));  // a = 79.95
    result = ArrangeByAngleAroundY(test);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . arranging around y, expected " << ManifoldText(check) << endl;
        cout << "                        got      " << ManifoldText(result) << endl;
        passed = false;
    }

    test.clear();
    test.push_back(Vector3d( 0.17,  0.76, -0.89));  // a = -79.19
    test.push_back(Vector3d( 0.07, -0.55,  0.58));  // a = 83.12
    test.push_back(Vector3d(-0.99,  0.50,  0.74));  // a = 143.22
    test.push_back(Vector3d( 0.76,  0.43, -0.34));  // a = -24.10
    test.push_back(Vector3d(-0.24, -0.44,  0.29));  // a = 129.61
    check.clear();
    check.push_back(Vector3d( 0.17,  0.76, -0.89));  // a = -79.19
    check.push_back(Vector3d( 0.76,  0.43, -0.34));  // a = -24.10
    check.push_back(Vector3d( 0.07, -0.55,  0.58));  // a = 83.12
    check.push_back(Vector3d(-0.24, -0.44,  0.29));  // a = 129.61
    check.push_back(Vector3d(-0.99,  0.50,  0.74));  // a = 143.22
    result = ArrangeByAngleAroundY(test);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . arranging around y, expected " << ManifoldText(check) << endl;
        cout << "                        got      " << ManifoldText(result) << endl;
        passed = false;
    }

    test.clear();
    test.push_back(Vector3d( 0.34, -0.41, -0.02));  // a = -3.37
    test.push_back(Vector3d( 0.77, -0.06,  0.65));  // a = 40.17
    test.push_back(Vector3d(-0.61,  0.83,  0.18));  // a = 163.56
    test.push_back(Vector3d( 0.68,  0.75, -0.45));  // a = -33.50
    test.push_back(Vector3d(-0.11, -0.62, -0.05));  // a = -155.56
    check.clear();
    check.push_back(Vector3d(-0.11, -0.62, -0.05));  // a = -155.56
    check.push_back(Vector3d( 0.68,  0.75, -0.45));  // a = -33.50
    check.push_back(Vector3d( 0.34, -0.41, -0.02));  // a = -3.37
    check.push_back(Vector3d( 0.77, -0.06,  0.65));  // a = 40.17
    check.push_back(Vector3d(-0.61,  0.83,  0.18));  // a = 163.56
    result = ArrangeByAngleAroundY(test);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . arranging around y, expected " << ManifoldText(check) << endl;
        cout << "                        got      " << ManifoldText(result) << endl;
        passed = false;
    }

    // CHECK AROUND X
    test.clear();
    test.push_back(Vector3d( 0.21, -0.93,  0.79));  // a = -49.65
    test.push_back(Vector3d(-0.91,  0.09,  0.23));  // a = 21.37
    test.push_back(Vector3d(-0.33,  0.95,  0.73));  // a = 52.46
    test.push_back(Vector3d( 0.24, -0.38, -0.82));  // a = -155.14
    test.push_back(Vector3d( 0.11,  0.40, -0.06));  // a = 98.53
    check.clear();
    check.push_back(Vector3d( 0.24, -0.38, -0.82));  // a = -155.14
    check.push_back(Vector3d( 0.21, -0.93,  0.79));  // a = -49.65
    check.push_back(Vector3d(-0.91,  0.09,  0.23));  // a = 21.37
    check.push_back(Vector3d(-0.33,  0.95,  0.73));  // a = 52.46
    check.push_back(Vector3d( 0.11,  0.40, -0.06));  // a = 98.53
    result = ArrangeByAngleAroundX(test);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . arranging around x, expected " << ManifoldText(check) << endl;
        cout << "                        got      " << ManifoldText(result) << endl;
        passed = false;
    }

    test.clear();
    test.push_back(Vector3d( 0.27,  0.35, -0.49));  // a = 144.46
    test.push_back(Vector3d( 0.33, -0.52, -0.88));  // a = -149.42
    test.push_back(Vector3d( 0.45, -0.08,  0.82));  // a = -5.57
    test.push_back(Vector3d(-0.06, -0.02, -0.85));  // a = -178.65
    test.push_back(Vector3d(-0.63, -0.97,  0.01));  // a = -89.41
    check.clear();
    check.push_back(Vector3d(-0.06, -0.02, -0.85));  // a = -178.65
    check.push_back(Vector3d( 0.33, -0.52, -0.88));  // a = -149.42
    check.push_back(Vector3d(-0.63, -0.97,  0.01));  // a = -89.41
    check.push_back(Vector3d( 0.45, -0.08,  0.82));  // a = -5.57
    check.push_back(Vector3d( 0.27,  0.35, -0.49));  // a = 144.46
    result = ArrangeByAngleAroundX(test);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . arranging around x, expected " << ManifoldText(check) << endl;
        cout << "                        got      " << ManifoldText(result) << endl;
        passed = false;
    }

    test.clear();
    test.push_back(Vector3d( 0.74, -0.39,  0.01));  // a = -88.53
    test.push_back(Vector3d(-0.75, -0.27, -0.24));  // a = -131.63
    test.push_back(Vector3d(-0.20,  0.92,  0.87));  // a = 46.60
    test.push_back(Vector3d( 0.43, -0.91,  0.26));  // a = -74.05
    test.push_back(Vector3d( 0.61,  0.35, -0.58));  // a = 148.89
    check.clear();
    check.push_back(Vector3d(-0.75, -0.27, -0.24));  // a = -131.63
    check.push_back(Vector3d( 0.74, -0.39,  0.01));  // a = -88.53
    check.push_back(Vector3d( 0.43, -0.91,  0.26));  // a = -74.05
    check.push_back(Vector3d(-0.20,  0.92,  0.87));  // a = 46.60
    check.push_back(Vector3d( 0.61,  0.35, -0.58));  // a = 148.89
    result = ArrangeByAngleAroundX(test);
    if (!CompareManifolds(result, check))
    {
        if (!linedown) {linedown = true; cout << endl;}
        cout << "  . arranging around x, expected " << ManifoldText(check) << endl;
        cout << "                        got      " << ManifoldText(result) << endl;
        passed = false;
    }

    return passed;
}



// Program entry and testing structure
int main()
{

    cout << "Checking vector creation...                       ";
    if (VectorCreation())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking vector lengths/distances...              ";
    if (VectorLengths())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking vector operators...                      ";
    if (VectorOperators())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking vector unit and dot product...           ";
    if (VectorUnitAndDot())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking vector rotations...                      ";
    if (VectorRotations())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking vector angles...                         ";
    if (VectorAngles())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking vector projection...                     ";
    if (VectorProjection())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking manifold length...                       ";
    if (CheckManifoldLength())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking on segment detection...                  ";
    if (CheckIsOnSegment())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking preceeding point indexing...             ";
    if (CheckManifoldPreceedingPointIndex())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking manifold splitting...                    ";
    if (CheckManifoldSplit())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking manifold closest point detection...      ";
    if (CheckClosestPoints())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking vector intersections...                  ";
    if (CheckIntersections())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking removal and resampling...                ";
    if (CheckRemoveAndResample())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking points along manifold...                 ";
    if (CheckPointsAlongManifold())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking cartesian and radial filtering...        ";
    if (CheckFilteringExtremes())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking radial highest/lowest...                 ";
    if (CheckRadialExtremes())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    cout << "Checking angular sorting...                       ";
    if (CheckArrangeByAngle())
        cout << "passed" << endl;
    else
        cout << "FAILED" << endl;

    return 0;
}