CC=g++
CFLAGS=-O3 -std=gnu++11

vector_tests: vector3d.cpp vector_tests.cpp vector3d.h
	$(CC) vector3d.cpp vector_tests.cpp -o bin/vector_tests $(CFLAGS)
