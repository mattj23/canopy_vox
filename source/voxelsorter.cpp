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

#include "voxelsorter.h"
#include <iostream>
#include <cmath>

VoxelAddress::VoxelAddress(const int _i, const int _j, const int _k)
{
    i = _i;
    j = _j;
    k = _k;
}

VoxelAddress::VoxelAddress()
{
    i = 0;
    j = 0;
    k = 0;
}

::std::ostream& operator<<(::std::ostream& os, const VoxelAddress& a)
{
    return os << "VoxelAddress(" << a.i << ", " << a.j << ", " << a.k << ")";
}

LocatedPoint::LocatedPoint(const Vector3d& p, const VoxelAddress& a)
{
    location = p;
    address = a;
}

VoxelSorter::VoxelSorter(double di, double dj, double dk, double i0, double j0, double k0)
{
    ispan = di;
    jspan = dj;
    kspan = dk;
    izero = i0;
    jzero = j0;
    kzero = k0;
}

LocatedPoint VoxelSorter::identifyPoint(const Vector3d& point) const
{
    int i_ = static_cast<int>(std::floor((point.x - izero) / ispan));
    int j_ = static_cast<int>(std::floor((point.y - jzero) / jspan));
    int k_ = static_cast<int>(std::floor((point.z - kzero) / kspan));

    return LocatedPoint(point, VoxelAddress(i_, j_, k_));
}
