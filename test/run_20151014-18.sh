#!/bin/bash

outfile="./output/all_file_size.log"

for x in listfile/*; do
	echo $x | tee -a $outfile
	./bin/SCI_Decode $x | tee -a $outfile
	./test/test_gps_time | tee -a $outfile
	./test/test_cnt | tee -a $outfile
	echo "########## DELIMETER ##########" | tee -a $outfile
done
