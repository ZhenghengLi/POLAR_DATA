#!/bin/sh

outfile="./output/out_err_check.log"

for x in /home/mark/IHEP_Work/POLAR/My_Work/Decode_Project/listfile/*; do
	echo ${x##*/} | tee -a $outfile
	./bin/main $x | tee -a $outfile
	./test/check_error.py ./output/test.log | tee -a $outfile
	echo "########## DELIMETER ##########" | tee -a $outfile
done
