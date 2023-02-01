#!/bin/bash

rm -rf build bin

echo "Build $1"

cp CMakeLists_Server.txt CMakeLists.txt

cmake -S . -B build
cmake --build build -v

cd build
cmake --install .
cd ..
mkdir bin/PKI
cp -R PKI bin/
