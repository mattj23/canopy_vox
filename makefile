CC=g++
MPICC=mpic++
CFLAGS=-O2 -std=gnu++11
LTESTFLAGS= -lgtest -lpthread # Link flags for Google Testing Framework
BIN=./bin/
SRC=./source/

mpi_voxels: $(SRC)mpi_voxels.cpp $(BIN)jsoncpp.o $(BIN)utilities.o $(BIN)vector3d.o $(BIN)pointcloud.o $(BIN)voxelsorter.o
	$(MPICC) $(SRC)mpi_voxels.cpp $(BIN)jsoncpp.o $(BIN)utilities.o $(BIN)vector3d.o $(BIN)pointcloud.o $(BIN)voxelsorter.o -o $(BIN)mpi_test $(CFLAGS)

kdtree_voxels: $(SRC)kdtree_voxels.cpp $(BIN)pointcloud.o $(BIN)vector3d.o $(BIN)utilities.o $(BIN)jsoncpp.o $(BIN)voxelsorter.o
	$(CC) $(SRC)kdtree_voxels.cpp $(BIN)vector3d.o $(BIN)pointcloud.o $(BIN)utilities.o $(BIN)jsoncpp.o $(BIN)voxelsorter.o -o $(BIN)kdtree_voxels $(CFLAGS)

naive_voxels: $(SRC)naive_voxels.cpp $(BIN)vector3d.o $(BIN)utilities.o $(BIN)jsoncpp.o $(BIN)voxelsorter.o
	$(CC) $(SRC)naive_voxels.cpp $(BIN)vector3d.o $(BIN)utilities.o $(BIN)jsoncpp.o $(BIN)voxelsorter.o -o $(BIN)naive_voxels $(CFLAGS)

closest_point_check: $(SRC)closest_point_check.cpp $(BIN)pointcloud.o $(BIN)vector3d.o $(BIN)utilities.o $(BIN)jsoncpp.o $(BIN)voxelsorter.o
	$(CC) $(SRC)closest_point_check.cpp $(BIN)vector3d.o $(BIN)pointcloud.o $(BIN)utilities.o $(BIN)jsoncpp.o $(BIN)voxelsorter.o -o $(BIN)closest_point_check $(CFLAGS)


$(BIN)pointcloud.o: $(SRC)pointcloud.h $(SRC)pointcloud.cpp $(BIN)vector3d.o
	$(CC) $(SRC)pointcloud.cpp $(BIN)vector3d.o -c -o $(BIN)pointcloud.o $(CFLAGS)

$(BIN)utilities.o: $(SRC)utilities.cpp $(SRC)utilities.h $(BIN)vector3d.o $(BIN)jsoncpp.o
	$(CC) $(SRC)utilities.cpp $(BIN)vector3d.o $(BIN)jsoncpp.o -c -o $(BIN)utilities.o $(CFLAGS)

$(BIN)jsoncpp.o: $(SRC)jsoncpp.cpp # $(SRC)json/json.h
	$(CC) $(SRC)jsoncpp.cpp -c -o $(BIN)jsoncpp.o $(CFLAGS)

$(BIN)voxel_tests: $(BIN)voxelsorter.o $(SRC)test_voxelsorter.cpp $(BIN)vector3d.o
	$(CC) $(SRC)test_voxelsorter.cpp $(BIN)voxelsorter.o $(BIN)vector3d.o -o $(BIN)voxel_tests $(CFLAGS) $(LTESTFLAGS)

$(BIN)voxelsorter.o: $(SRC)voxelsorter.cpp $(SRC)voxelsorter.h $(BIN)vector3d.o
	$(CC) $(SRC)voxelsorter.cpp $(BIN)vector3d.o -c -o $(BIN)voxelsorter.o $(CFLAGS)

$(BIN)pointcloud_tests: $(BIN)pointcloud.o $(SRC)test_pointcloud.cpp $(BIN)vector3d.o
	$(CC) $(BIN)pointcloud.o $(SRC)test_pointcloud.cpp $(BIN)vector3d.o -o $(BIN)pointcloud_tests $(CFLAGS) $(LTESTFLAGS)

# g++ test_vector3d.cpp vector3d.cpp -o bin/gtest --std=gnu++11 -lgtest -lpthread
$(BIN)vector_tests: $(BIN)vector3d.o $(SRC)test_vector3d.cpp
	$(CC) $(SRC)test_vector3d.cpp $(BIN)vector3d.o -o $(BIN)vector_tests $(CFLAGS) $(LTESTFLAGS)

$(BIN)vector3d.o: $(SRC)vector3d.cpp $(SRC)vector3d.h
	$(CC) $(SRC)vector3d.cpp -c -o $(BIN)vector3d.o $(CFLAGS)

alltests: $(BIN)voxel_tests $(BIN)vector_tests $(BIN)pointcloud_tests

runtests: alltests
	$(BIN)vector_tests
	$(BIN)voxel_tests
	$(BIN)pointcloud_tests

clean:
	\rm $(BIN)*
