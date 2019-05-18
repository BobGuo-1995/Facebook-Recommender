.SUFFIXES: .cpp

CC = g++
CFLAGS = -O3 -g -I/usr/local/include -std=c++11

OBJ1 = matrix.o
OBJ2 = adjacency.o

all: matrix adjacency

matrix: $(OBJ1)
	$(CC) $(CFLAGS) -o matrix $(OBJ1)

adjacency: $(OBJ2)
	$(CC) $(CFLAGS) -o adjacency $(OBJ2) $(LDIR)

.cpp.o:
	$(CC) $(CFLAGS) -c $?

clean:
	rm -f matrix adjacency $(OBJ1) $(OBJ2)
