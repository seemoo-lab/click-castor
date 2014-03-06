#!/usr/bin/python
import sys

print('Setting Neigbor Filter to ' + str(sys.argv[1]))

with open("mycastorrouter2.click", "wt") as fout:
    with open("mycastorrouter.click", "rt") as fin:
        for line in fin:
            fout.write(line.replace('##FILT##', str(sys.argv[1])))

