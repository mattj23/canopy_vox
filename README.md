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

### Parallel Algorithm
The parallel algorithm works as follows:
1. The algorithm is invoked through MPI with a given number of processes.
2. Processes self-assign based on the total MPI world size and the number of input files.
   * Process #0 is always the Director
   * Up to 25% of the processes are allowed to become Readers.  There will always be at least one Reader, but never any more than the number of input files.
   * The remainder of the processes become Workers.
3. Readers begin the process of reading the input files from disk.  As points are loaded they are sorted into working bins based on a large discretization of space (typically cubic meter or larger bins) thats length, width, and height dimensions are integer multiples of the final bin space, then shifted by half.  The worker assigned to each bin is deterministically calculated from a hash of the bin address, and the bins are assembled and the data transfered from the Readers to the Workers.
4. When the Readers have finished their reading, they transmit a completed signal to the Director.  When all Readers have indicated completion the Director broadcasts a message to all workers indicating that the data has all been loaded.
5. Each Worker goes through each working bin and constructs a 3-d search tree of each space, then performs a thinning operation to remove redundant points within that volume.
6. Next, the thinned points are written to a scratch directory, and the workers signal to the Director that they have finished their task.
7. When all Workers have been accounted for, the Director signals the Readers to begin reading again.  The process is repeated, except that this time the origin of the voxel space is centered at 0, 0, 0.  This ensures that regions of space that lay at the boundaries of the original bins, and hence points that had redundant neighbors in an adjacent region, are now in the same thinning workspace.  Process steps 3-5 are repeated otherwise unchanged.
8. When all Workers have completed the secondary thinning operation, each bin is subdivided into the final voxels and the point count for each final voxel is computed. Each worker writes a file with a sparse representation of their voxel space to disk.

### Testing
Testing is done with Google Test, and if the binaries are installed can be built and run with the included makefile: `make runtests`

### Dependencies and Acknowledgements
1. **jsoncpp**, by Baptiste Lepilleur, used for parsing the json-formatted configuration files. (MIT license)
2. **nanoflann**, by Jose Luis Blanco-Claraco, a k-d tree implementation used for distance searches within the point cloud, a fork of the FLANN project (BSD license)
3. **googletest**, by Google, a unit testing framework used for testing the vector and voxel libraries
