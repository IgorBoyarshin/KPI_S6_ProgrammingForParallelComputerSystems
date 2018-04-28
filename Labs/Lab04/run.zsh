#!/usr/bin/zsh

export OMP_STACKSIZE=400000

make

./main

rm -f *.o
rm -f main
