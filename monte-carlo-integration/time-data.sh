#!/bin/bash

if [ "$#" != 4 ]; then
	echo "usage: $0 a b n outfile"
	exit 1
fi

a=$1; b=$2; n=$3; outfile=$4

echo "n_threads,time" > $outfile

for n_threads in {1..40}
do
	./integrate $a $b $n $n_threads $outfile
done

./plot.py $outfile $(echo $outfile | sed 's/\..*$//')
