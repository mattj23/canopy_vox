CC=g++
CFLAGS=-O2 -std=gnu++11
LTESTFLAGS= -lgtest -lpthread # Link flags for Google Testing Framework
BIN=./bin/
SRC=./source/

naive_voxels: $(SRC)naive_voxels.cpp $(BIN)vector3d.o $(BIN)utilities.o
	$(CC) $(SRC)naive_voxels.cpp $(BIN)vector3d.o $(BIN)utilities.o -o $(BIN)naive_voxels $(CFLAGS)

$(BIN)utilities.o: $(SRC)utilities.cpp $(SRC)utilities.h $(BIN)vector3d.o
	$(CC) $(SRC)utilities.cpp $(BIN)vector3d.o -c -o $(BIN)utilities.o $(CFLAGS)

$(BIN)voxel_tests: $(BIN)voxelsorter.o $(SRC)test_voxelsorter.cpp $(BIN)vector3d.o
	$(CC) $(SRC)test_voxelsorter.cpp $(BIN)voxelsorter.o $(BIN)vector3d.o -o $(BIN)voxel_tests $(CFLAGS) $(LTESTFLAGS)

$(BIN)voxelsorter.o: $(SRC)voxelsorter.cpp $(SRC)voxelsorter.h $(BIN)vector3d.o
	$(CC) $(SRC)voxelsorter.cpp $(BIN)vector3d.o -c -o $(BIN)voxelsorter.o $(CFLAGS)

# g++ test_vector3d.cpp vector3d.cpp -o bin/gtest --std=gnu++11 -lgtest -lpthread
$(BIN)vector_tests: $(BIN)vector3d.o $(SRC)test_vector3d.cpp
	$(CC) $(SRC)test_vector3d.cpp $(BIN)vector3d.o -o $(BIN)vector_tests $(CFLAGS) $(LTESTFLAGS)

$(BIN)vector3d.o: $(SRC)vector3d.cpp $(SRC)vector3d.h
	$(CC) $(SRC)vector3d.cpp -c -o $(BIN)vector3d.o $(CFLAGS)

alltests: $(BIN)voxel_tests $(BIN)vector_tests

runtests: alltests
	$(BIN)vector_tests
	$(BIN)voxel_tests

clean:
	\rm $(BIN)*
