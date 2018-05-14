#!/usr/bin/zsh

javac Vector.java
javac Matrix.java
javac MonitorSynchronization.java
javac MonitorResources.java
javac Main.java

java -Xss10M Main

rm -f *.class
