# canopy_vox
Parallel computing voxel generator from LIDAR data

This code performs the voxelization of a very large volume of LIDAR points using a parallel algorithm and Open MPI.

It was written for use on Georgia Southern University's TALON cluster.

## Dependencies and Acknowledgements
1. **jsoncpp**, by Baptiste Lepilleur, used for parsing the json-formatted configuration files.
2. **nanoflann**, by Jose Luis Blanco-Claraco, a k-d tree implementation used for distance searches within the point cloud, a fork of the FLANN project
