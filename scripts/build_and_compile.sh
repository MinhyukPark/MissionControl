#!/bin/sh
rm -r build
mv tmp tmp_old
mv input input_old
mv errors errors_old
mv output output_old
mv run.log run.log.old
mkdir build
cd build
cmake ..
cp compile_commands.json ../
make -j 10
