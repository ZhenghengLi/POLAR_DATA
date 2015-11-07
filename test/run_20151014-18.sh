#!/bin/bash

for x in listfile/*; do
	./bin/main $x
	sx=${x:17:17}
	y=output/all_time/time${sx}log
	echo "output/test.log -> $y"
	cp output/test.log $y
	echo "***********************************************************"
done
