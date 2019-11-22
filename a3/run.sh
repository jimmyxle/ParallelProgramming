#!/bin/bash
# gcc -std=c11 -o seq_floyd.o seq_floyd.c
# ./seq_floyd.o
mpicc -std=c11 -o bcast_floyd.o bcast_floyd.c
mpirun -np 16 bcast_floyd.o 4
