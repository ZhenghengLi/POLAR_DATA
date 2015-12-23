#!/bin/bash

outfile="./output/all_lost_comp.log"

for x in listfile/*; do
    echo $x | tee -a $outfile
    ./bin/SCI_Decode $x | tee -a $outfile
    ./test/test_lost | tee -a $outfile
    echo "########## DELIMETER ##########" | tee -a $outfile
done
