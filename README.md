# canopy_vox

### Overview
*Parallel computing voxel generator from LIDAR data*

This code performs the voxelization of a very large volume of LIDAR points using a parallel algorithm and Open MPI.

It was written for use on Georgia Southern University's TALON cluster.

### Current State
The current state of canopy_vox is a naive, single threaded implementation of the thinning (elimination of redundant points within a specified distance) and voxelization algorithms that takes a single .asc input file.

No intelligence is used when prioritizing which point is saved when thinning, and the thinning algorithm uses a naive nested loop to find neighbors within the thinning window.  

Currently the program can be compiled with the included makefile, and will produce a file `bin/naive_voxels`, which can be run from the command line with the configuration file specified as the first argument.

Configuration files are in the json format, and a sample `config.json` is included that points at the sample file `sample_data/sample.asc`.

### Dependencies and Acknowledgements
1. **jsoncpp**, by Baptiste Lepilleur, used for parsing the json-formatted configuration files. (MIT license)
2. **nanoflann**, by Jose Luis Blanco-Claraco, a k-d tree implementation used for distance searches within the point cloud, a fork of the FLANN project (BSD license)
3. **googletest**, by Google, a unit testing framework used for testing the vector and voxel libraries
