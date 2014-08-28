#!/usr/bin/python

import multiprocessing
import subprocess
import sys
import argparse
import os
from datetime import datetime
import numpy
import scipy.stats

# Settings: possible values are defined in 'castor-xcast.cc'
runs       = range(1,21)
duration   = 60.0 * 10.0
clicks     = ["xcast-promisc", "xcast", "regular"]
networks   = ["small", "medium"]#, "large"]
traffics   = ["20_1", "10_2", "4_5", "2_10"]
mobilities = ["0", "20"]
blackholes = [0.0, 0.2, 0.4]



def base_conf():
    network   = "medium"
    traffic   = "4_5"
    mobility  = "20"
    blackhole = 0.0
    return network, traffic, mobility, blackhole

def file_name(
              work_dir, 
              network=base_conf()[0], 
              traffic=base_conf()[1], 
              mobility=base_conf()[2], 
              blackhole=base_conf()[3]):
    return work_dir + network + "-" + traffic + "-" + mobility + "-" + `blackhole`

def generate_cmd(
                 work_dir, 
                 networks=[base_conf()[0]], 
                 traffics=[base_conf()[1]], 
                 mobilities=[base_conf()[2]], 
                 blackholes=[base_conf()[3]]):
    """Generates commands for running experiments in ns-3 
    """
    if not os.path.exists(os.path.dirname(work_dir)):  
        os.makedirs(os.path.dirname(work_dir)) 
    
    return [["./waf", "--run", "castor-xcast",
            "--command-template",
            "%s --run="    + `run` +
            " --duration=" + `duration` +
            " --click="    + click +
            " --network="  + network +
            " --traffic="  + traffic +
            " --mobility=" + mobility +
            " --blackholes=" + `blackhole` +
            " --outfile="  + file_name(work_dir, network, traffic, mobility, blackhole) + "-" + click + "-" + `run`
            ]
           for run       in runs
           for click     in clicks
           for network   in networks
           for traffic   in traffics
           for mobility  in mobilities
           for blackhole in blackholes
           ]

def generate_all_cmd(work_dir):
    all_cmds = []
    # Add others (excluding default config)
    all_cmds.extend(generate_cmd(work_dir, networks=networks))
    all_cmds.extend(generate_cmd(work_dir, traffics=traffics))
    all_cmds.extend(generate_cmd(work_dir, mobilities=mobilities))
    all_cmds.extend(generate_cmd(work_dir, blackholes=blackholes))
    
    # remove all occurences of the default config
    default_cmd = generate_cmd(work_dir)
    all_cmds[:] = [x for x in all_cmds if not x == default_cmd]
    all_cmds.extend(default_cmd) # Add default config

    return all_cmds

def evaluate(work_dir):
    for network in networks:
        average_runs(file_name(work_dir, network=network))
    for traffic in traffics:
        average_runs(file_name(work_dir, traffic=traffic))
    for mobility in mobilities:
        average_runs(file_name(work_dir, mobility=mobility))
    for blackhole in blackholes:
        average_runs(file_name(work_dir, blackhole=blackhole))

def average_runs(fileprefix, clicks=clicks):
    out_file = file(fileprefix, "w")
    # Clear old file
    out_file.write("")
    out_file.close()
    for click in clicks:
        # Read out metrics from all runs
        accum = numpy.array([])
        for i in runs:
            current_file_name = fileprefix + "-" + click + "-" + `i`
            current_file = file(current_file_name, "r")
            line = map(num, current_file.readline().split(" "))
            current_file.close()
            if accum.any():
                accum = numpy.vstack((accum, numpy.array(line)))
            else:
                accum = numpy.array(line)

        result = []
        if(accum.ndim > 1):
            # Append mean and confidence interval for every metric
            for i in range(accum.shape[1]):
                m, h = mean_confidence_interval(accum[:,i])
                result.append(`m` + " " + `h`)
        else:
            result = accum
        # Write results for this Click configuration
        out_file = file(fileprefix, "a")
        out_file.write(click)
        for x in result:
            out_file.write(" " + str(x))
        out_file.write("\n")
        out_file.close()
    
def num(s):
    """Converts a string to a number value (int or float)
    """
    try:
        return int(s)
    except ValueError:
        return float(s)

def mean_confidence_interval(data, confidence=0.95):
    n = len(data)
    m, se = numpy.mean(data), scipy.stats.sem(data)
    h = se * scipy.stats.t._ppf((1+confidence)/2., n-1) # The confidence interval
    return m, h

def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--comment", help="Note to the experiment to be run")
    args = parser.parse_args(argv)

    # Create workdir
    comment = "_" + args.comment if args.comment else ""
    i = datetime.now()
    work_dir = "out/" + i.strftime('%Y-%m-%d_%H.%M.%S') + comment + "/"
    if not os.path.exists(os.path.dirname(work_dir)):  
        os.makedirs(os.path.dirname(work_dir)) 

    print "Pre-Build experiment"
    result = subprocess.call(["./waf", "build"])
   
    if result:
        print >>sys.stderr, "Failed to build experiment, stop"
        return result
  
    print "Start experiments on " + `multiprocessing.cpu_count()` + " core(s)"
    pool = multiprocessing.Pool(None) # use 'multiprocessing.cpu_count()' cores
    pool.map_async(subprocess.call, generate_all_cmd(work_dir))
    pool.close()
    pool.join()
 
    print "Evaluate experiments"
    evaluate(work_dir)
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
    