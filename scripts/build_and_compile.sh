#!/bin/sh
rm -r build
mv tmp tmp_old
mv input input_old
mv errors errors_old
mv output output_old
mkdir build
cd build
export CC=/usr/bin/gcc
export CXX=/usr/bin/g++
cmake ..
cp compile_commands.json ../
make -j 10
