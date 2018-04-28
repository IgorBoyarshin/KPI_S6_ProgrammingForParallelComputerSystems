#!/usr/bin/zsh

# gcc -c lab1.adb -gnatl=llab1.lst
# gcc -c data.adb -gnatl=ldata.lst

gnatmake lab5 -gnatl=lst.lst
./lab5

# rm -f *.o
rm -f lab5
