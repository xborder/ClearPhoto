#!/usr/bin/python

import sys
import math
import numpy

files = sys.argv[1:]
times = []
for file in files:
	lines = [line.strip() for line in open(file)]
	times.append(int(lines[4].split('=')[1])*0.001)

times.sort()
times = times[1:-1]
avg = sum(times)/len(times)
print "avg: " + str(avg)
print "std: " + str(numpy.std(times, axis=0))