#!/usr/bin/zsh

# gcc -c lab1.adb -gnatl=llab1.lst
# gcc -c data.adb -gnatl=ldata.lst

gnatmake lab1 -gnatl=lst.lst
./lab1

# rm -f *.o
rm -f lab1
