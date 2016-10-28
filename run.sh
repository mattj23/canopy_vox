rm ~/voxel/output/*
rm ~/voxel/scratch/*
mpirun -np 5 ./bin/mpi_test parallel_config.json
