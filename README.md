# canopy_vox

### Overview
*Parallel computing voxel generator from LIDAR data*

This code performs the voxelization of a very large volume of LIDAR points using a parallel algorithm and Open MPI.

It was written for use on Georgia Southern University's TALON cluster.

### Current State
There are currently several binaries in the project.

1. kdtree_voxels

   This is a single threaded implementation of the thinning and voxelization algorithms that uses a 3 dimensional binary search tree (a k-d tree) to perform the spatial radius searches in O(n log n) time as opposed to the O(n^2) time of the naive implementation.  Takes a single .asc input file.  Works for point clouds up to a few million points.

2. naive_voxels

   This is a naive, single threaded implementation of the thinning (elimination of redundant points within a specified distance) and voxelization algorithms that takes a single .asc input file.  It is very slow for point clouds over a few thousand points, and exists solely as an obviously correct method for validating the output of the other implementations.

Configuration files are in the json format, and a sample `config.json` is included that points at the sample file `sample_data/sample.asc`.

### Testing
Testing is done with Google Test, and if the binaries are installed can be built and run with the included makefile: `make runtests`

### Dependencies and Acknowledgements
1. **jsoncpp**, by Baptiste Lepilleur, used for parsing the json-formatted configuration files. (MIT license)
2. **nanoflann**, by Jose Luis Blanco-Claraco, a k-d tree implementation used for distance searches within the point cloud, a fork of the FLANN project (BSD license)
3. **googletest**, by Google, a unit testing framework used for testing the vector and voxel libraries
