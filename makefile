CC=g++
CFLAGS=-O3 -std=gnu++11
LTESTFLAGS= -lgtest -lpthread

# g++ test_vector3d.cpp vector3d.cpp -o bin/gtest --std=gnu++11 -lgtest -lpthread
vector_tests: vector3d.cpp test_vector3d.cpp vector3d.h
	$(CC) vector3d.cpp test_vector3d.cpp -o bin/vector_tests $(CFLAGS) $(LTESTFLAGS)
