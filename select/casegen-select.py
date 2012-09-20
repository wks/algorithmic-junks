#!/usr/bin/python2

import random
import sys

sz,nth = map(int,sys.argv[1:3])

nums = range(1,sz+1)
random.shuffle(nums)

print sz, nth
for n in nums:
    print n,
