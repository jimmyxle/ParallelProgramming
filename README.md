### How to run ###


## For A3 ##
# Sequential Floyd #

gcc -std=c11 -o seq_floyd.o seq_floyd.c
./seq_floyd.o

# MPI Broadcast Floyd #

mpicc -std=c11 -o bcast_floyd.o bcast_floyd.c
mpirun -np 400 bcast_floyd.o 20

# MPI Pipeline Floyd #

mpicc -std=c11 -o pipe_floyd.o pipe_floyd.c
mpirun -np 3 pipe_floyd.o 3
