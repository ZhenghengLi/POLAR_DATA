#!/usr/bin/python

import sys
import numpy as np
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print "USAGE: " + sys.argv[0] + " <infile_name>"
    exit(1)

infile_name = sys.argv[1]

x = []
y = []
for (n, line) in enumerate(open(infile_name, 'r')):
    x.append(n)
    y.append(float(line.rstrip()))

plt.plot(x, y)
plt.show()
