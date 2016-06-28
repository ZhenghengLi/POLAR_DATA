#!/bin/bash

tmp_dir=$(dirname ${BASH_SOURCE[0]})
my_dir=$(cd $tmp_dir > /dev/null ; pwd)

my_path=$my_dir/bin:$my_dir/Analysis/bin:$my_dir/Orbit_Split/bin:$my_dir/L1_Convert/bin:$my_dir/tools/bin
my_lib=$my_dir/Analysis/bin

if [[ $PATH =~ $my_path ]]; then
    echo "POLAR_RawData_Decode_Project is already in PATH"
else
    export PATH=$my_path:$PATH
    export export LD_LIBRARY_PATH=$my_lib:$LD_LIBRARY_PATH
    echo "Added POLAR_RawData_Decode_Project into PATH"
fi

