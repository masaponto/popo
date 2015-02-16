#!/bin/bash

cd ./src/test
if [ ! -d gtest-1.7.0 ]; then
    wget https://googletest.googlecode.com/files/gtest-1.7.0.zip
    unzip gtest-1.7.0.zip
fi
cd gtest-1.7.0
mkdir -p build
cd build
cmake ..
make
cd ../../
mkdir -p build
cd build
cmake ..
make
pwd
./popo_test
