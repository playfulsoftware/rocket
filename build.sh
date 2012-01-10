#!/bin/sh

clang -c src/main.cpp -o src/main.o
clang -o engine -lstdc++ src/main.o
