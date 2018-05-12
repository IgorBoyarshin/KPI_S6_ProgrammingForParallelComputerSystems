#!/usr/bin/zsh

mpiCC -o main main.cpp
mpirun --oversubscribe -np 12 ./main

rm -f main
