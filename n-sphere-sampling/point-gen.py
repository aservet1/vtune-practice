from sys import argv
import numpy as np
from random import random
from math import sqrt

# source: http://extremelearning.com.au/how-to-generate-uniformly-random-points-on-n-spheres-and-n-balls/

def verify_point(point):
	return sqrt(sum([ p**2 for p in point ])) <= 1

d = int(argv[1])

u = np.random.normal(0,1,d)  # an array of d normally distributed random variables
norm=np.sum(u**2) **(0.5)
r = random()**(1.0/d)
point = r*(u/norm)

verify_point(point)

print(point, verify_point(point))
