#!/usr/bin/zsh

mcs -out:Main *.cs
mono Main


rm -f Main
