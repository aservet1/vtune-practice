#!/bin/bash
if [ -z $1 ]
then
	echo 'pass an arg'
	exit
fi

echo y | rm $1
