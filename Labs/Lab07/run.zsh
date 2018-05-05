#!/usr/bin/zsh

gcc -c main.adb -gnatl=main.lst
# gcc -c data.adb -gnatl=ldata.lst

gnatmake main -gnatl=lst.lst
./main

rm -f *.o
rm -f *.ali
rm -f main
