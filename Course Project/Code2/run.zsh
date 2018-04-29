#!/usr/bin/zsh

mpiCC -o main main.cpp
mpirun --oversubscribe -np 4 ./main

# rm -f *.o
rm -f main
