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

Copyright (C) 2016   Matthew Jarvis

	A VoxelAddress is the i,j,k indicies of a voxel in the discretized space

    A LocatedPoint is a combination of a Vector3d and a VoxelAddress, and
    represents a point in 3-space which was identified as being contained in the
    voxel at the given address.  Note that the parameters of the bin
    discretization are not stored with the point.

    The VoxelSorter class is instantiated with the parameters of spatial
    discretization and will then take any Vector3d and determine the voxel
    address of the point, producing a LocatedPoint

*/
#ifndef VOXELSORTER_H
#define VOXELSORTER_H

#include "vector3d.h"
#include <iostream>

struct VoxelAddress
{
    int i;
    int j;
    int k;

    VoxelAddress();
    VoxelAddress(const int _i, const int _j, const int _k);
};

namespace std
{
    // Hash function for voxel address
    template <> struct hash<VoxelAddress>
    {
        inline size_t operator()(const VoxelAddress& x) const
        {
            size_t hash = x.i;
            hash *= 37;
            hash += x.j;
            hash *= 37;
            hash += x.k;
            return hash;
        }
    };
}

::std::ostream& operator<<(::std::ostream& os, const VoxelAddress& a);

inline bool operator==(const VoxelAddress& lhs, const VoxelAddress& rhs) {return lhs.i == rhs.i && lhs.j == rhs.j && lhs.k == rhs.k;}
inline bool operator!=(const VoxelAddress& lhs, const VoxelAddress& rhs) {return !operator==(lhs, rhs);}


struct LocatedPoint
{
    Vector3d location;
    VoxelAddress address;

    LocatedPoint(const Vector3d& p, const VoxelAddress& a);
};

class VoxelSorter
{
public:
    VoxelSorter(double di, double dj, double dk, double i0, double j0, double k0);
    LocatedPoint identifyPoint(const Vector3d& point) const;

protected:
    double ispan;
    double jspan;
    double kspan;
    double izero;
    double jzero;
    double kzero;
};
#endif
