#!/usr/bin/env python3

from sys import argv
from itertools import product as cartesian_product

if len(argv) != 2:
	print("usage: "+argv[0]+" data.json")
	exit(2)

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
import json
from scipy.interpolate import griddata

with open(argv[1]) as f: jsondata = json.loads(f.read())

x = sorted([ x for x in list(jsondata.keys()) ], key=int)
y = list(jsondata[x[0]].keys())

data = [ (float(c[0]),float(c[1]), jsondata[c[0]][c[1]] ) for c in cartesian_product(x,y) ]
x, y, z = zip(*data)
z = list(map(float, z))
grid_x, grid_y = np.mgrid[min(x):max(x):100j, min(y):max(y):100j]
grid_z = griddata((x, y), z, (grid_x, grid_y), method='cubic')

fig = plt.figure()
ax = fig.gca(projection='3d')
ax.plot_surface(grid_x, grid_y, grid_z)#, cmap=cm.coolwarm)

ax.set_xlabel('dimension')
ax.set_ylabel('distance from surface')

plt.show()
