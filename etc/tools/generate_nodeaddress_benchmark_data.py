#!/usr/bin/env python3

import sys
from math import floor
from numpy.random import default_rng

rng = default_rng()

if len(sys.argv) > 1:
    limit = int(sys.argv[1])
else:
    limit = 1000

count = 0

lines = set()

with open('datatree_utest_addressBenchmarkData.txt', 'w') as f:
    while count < limit:
        length = floor(rng.exponential(4)) + 1
        line = ''
        
        for i in range(length):
            x = floor(rng.exponential(32))
            line += str(x)
            if i < length - 1:
                line += ','
            
        if line in lines:
            continue
        
        lines.add(line)
        f.write(line + '\n')
        print(line)
        count += 1
