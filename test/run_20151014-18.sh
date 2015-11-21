#!/bin/bash

outfile="./output/all_merge_test.log"

for x in listfile/*; do
	./bin/main $x | tee -a $outfile
	./test/test | tee -a $outfile
	echo "########## DELIMETER ##########" | tee -a $outfile
done
