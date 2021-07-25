#!/bin/bash

# will probably have to reconfigure the run parameters (executable filename, args, etc) before running this again

outf=result.txt
cat /dev/null > $outf

for t in {0..8}; do
	echo " ))) threads: $t"                  | tee -a $outf
	./oUT $t ../data/data_13200837.dat x x   | tee -a $outf
	echo "---------------------------------" | tee -a $outf
done
