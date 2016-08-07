#!/bin/bash

tmp_dir=$(dirname ${BASH_SOURCE[0]})
my_dir=$(cd $tmp_dir > /dev/null ; pwd)

my_path=$my_dir/Analysis/bin:$my_dir/Analysis/script:$my_dir/Preprocessing/bin:$my_dir/Preprocessing/script:$my_dir/Tools/bin:$my_dir/Tools/script
my_lib=$my_dir/Analysis/bin

#BOOST_PREFIX=/opt/mark/Boost/Boost_1_61_0_old_ABI

if [[ $PATH =~ $my_path ]]; then
    echo "POLAR_RawData_Decode_Project is already in PATH"
else
    export PATH=$my_path:$PATH
    export LD_LIBRARY_PATH=$my_lib:$LD_LIBRARY_PATH
    if [ $BOOST_PREFIX ]; then
        export BOOST_INC=-I$BOOST_PREFIX/include
        export BOOST_LIB=-L$BOOST_PREFIX/lib
        export LD_LIBRARY_PATH=$BOOST_PREFIX/lib:$LD_LIBRARY_PATH
    fi
    echo "Added POLAR_RawData_Decode_Project into PATH"
fi

