#!/bin/bash
export CXX=/usr/gcc-9.3.0/bin/g++
export CC=/usr/gcc-9.3.0/bin/gcc
export LD_LIBRARY_PATH=/usr/gcc-9.3.0/lib64/:$LD_LIBRARY_PATH
cmake .  -DCMAKE_BUILD_TYPE=$1
make