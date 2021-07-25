#!/usr/bin/env python3

import pandas as pd
from sys import argv
import matplotlib.pyplot as plt

if len(argv) != 3:
	print("usage",argv[0],"filename plotname")
	exit(2)

filename = argv[1]
plotname = argv[2]

df = pd.read_csv(filename)

#print(df)

threads = df['n_threads']
times = df['time']

# make the plot for runtime
x = threads
y = times
plt.plot(x,y)
plt.title('runtimes')
plt.xlabel('threads')
plt.ylabel('runtime')
plt.savefig(plotname+'-runtime')

# make plot for speedup
t1 = times[0]
speedup = [ t1/tp for tp in times ]
x = threads
y = speedup
plt.clf()
plt.plot(x,y)
plt.title('speedups')
plt.xlabel('threads')
plt.ylabel('speedup')
plt.savefig(plotname+'-speedup')

# make plot for efficiency
efficiency = [ speedup[i]/threads[i] for i in range(len(threads)) ]
x = threads
y = efficiency
plt.clf()
plt.plot(x,y)
plt.title('efficiencies')
plt.xlabel('threads')
plt.ylabel('efficiency')
plt.savefig(plotname+'-efficiency')
