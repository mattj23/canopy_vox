CC=g++
CFLAGS=-O2 -std=gnu++11
LTESTFLAGS= -lgtest -lpthread
BIN=./bin/
SRC=./source/

voxelsorter.o: $(SRC)voxelsorter.cpp $(SRC)voxelsorter.h
	$(CC) $(SRC)voxelsorter.cpp -c -o $(BIN)voxelsorter.o $(CFLAGS)

# g++ test_vector3d.cpp vector3d.cpp -o bin/gtest --std=gnu++11 -lgtest -lpthread
vector_tests: $(BIN)vector3d.o $(SRC)test_vector3d.cpp
	$(CC) $(SRC)test_vector3d.cpp $(BIN)vector3d.o -o $(BIN)vector_tests $(CFLAGS) $(LTESTFLAGS)

$(BIN)vector3d.o: $(SRC)vector3d.cpp $(SRC)vector3d.h
	$(CC) $(SRC)vector3d.cpp -c -o $(BIN)vector3d.o $(CFLAGS)

clean:
	\rm $(BIN)*
