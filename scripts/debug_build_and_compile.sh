#!/bin/sh
rm -r build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cp compile_commands.json ../
make -j 10
