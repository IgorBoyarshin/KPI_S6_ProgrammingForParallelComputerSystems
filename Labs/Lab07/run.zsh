#!/usr/bin/zsh

# gcc -c lab1.adb -gnatl=llab1.lst
# gcc -c data.adb -gnatl=ldata.lst

gnatmake main -gnatl=lst.lst
./main

rm -f *.o
rm -f *.ali
rm -f main
