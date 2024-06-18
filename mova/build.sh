#!/bin/bash

mkdir -p build

FILES=(src/main.cpp)

clang++ -std=c++17 -fsanitize=address -Wall -g -o build/main ${FILES}