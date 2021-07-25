#!/usr/bin/env python3

# file format is a \n separated list of tuples, first is the query point, the others are the knn, starting from the fartest

from sys import argv

with open(argv[1]) as f:
	data = f.read().split('\n')

def validate_reverse_sorted(l):
	for i in range(1,len(l)):
		if l[i] > l[i-1]: return False
	return True

def totup(s):
	return [ float(x) for x in s.replace(')','').replace('(','').split(' ') if x != '' ]

pt = totup(data[0])
data = data[1:]
data = [ totup(d) for d in data if d != '']
dists = [ (d[0]-pt[0])**2 + (d[1]-pt[1])**2 for d in data ]  

print(validate_reverse_sorted(dists))
