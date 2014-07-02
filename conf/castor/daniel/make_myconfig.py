#!/usr/bin/python
import sys

with open("mycastorrouter.click", "wt") as fout:
    with open("castor_routing.click", "rt") as fin:
        for line in fin:
            fout.write(line.replace('XXX.XXX.XXX.XXX/XX', str(sys.argv[1])))

