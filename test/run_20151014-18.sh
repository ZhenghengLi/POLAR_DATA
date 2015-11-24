#!/bin/bash

outfile="./output/all_file_size.log"

for x in listfile/*; do
	echo $x | tee -a $outfile
	./bin/SCI_Decode $x | tee -a $outfile
	du -c `cat $x` | tail -n 1 | tee -a $outfile
	./test/test | tee -a $outfile
	echo "-------------------------------" | tee -a $outfile
	./test/eat_error.pl ./output/sci_test.log
	./test/mod_time_diff.py ./output/sci_test.log ./output/bad_sci_test.log | tee -a $outfile
	echo "########## DELIMETER ##########" | tee -a $outfile
done
