# Goal
This project was to implement Floyd-Warshall's all-pair shortest path algorithm in a weighted graph. Generates a graph that shows the minimum distance from any nodes to all other nodes in the graph.
https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm

### OpenMPI 
Open source high performance computing

Library used to handle message passing in this project
https://www.open-mpi.org/


# How to run #

## For A1, A2 and A3 ##
Compile the source code and run the object file.

### Sequential Floyd ###

gcc -std=c11 -o seq_floyd.o seq_floyd.c

./seq_floyd.o

### MPI Broadcast Floyd ###

mpicc -std=c11 -o bcast_floyd.o bcast_floyd.c

mpirun -np 400 bcast_floyd.o 20

### MPI Pipeline Floyd ###

mpicc -std=c11 -o pipe_floyd.o pipe_floyd.c

mpirun -np 3 pipe_floyd.o 3
