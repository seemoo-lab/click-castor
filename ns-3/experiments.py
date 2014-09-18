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
clicks     = ["xcast-promisc", "xcast", "regular", "flooding"]
networks   = ["small", "medium", "large"]
traffics   = ["20_1", "10_2", "4_5", "2_10", "8_5"]
traffics_groupsize = ["20_1", "10_2", "4_5", "2_10"]
traffics_numgroups = ["4_5", "8_5"]
mobilities = ["0", "20"]
blackholes = ["0.0", "0.2", "0.4"]

dictionary = {"xcast-promisc" : "Xcastor(promisc.)", "xcast" : "Xcastor", "regular" : "Castor", "flooding" : "Flooding",
              "small" : "small", "medium" : "medium", "large" : "large",
              "20_1" : "20->1", "10_2" : "10->2", "4_5" : "4->5", "2_10" : "2->10", "8_5" : "8->5",
              "0" : "static", "20" : "RandomWaypoint",
              "0.0" : "0", "0.2" : "20", "0.4" : "40",
              
              "network" : "Network size", "traffic" : "Group size", "mobility" : "Mobility", "numgroups" : "Number of groups/senders", "blackhole" : "Fraction of malicious nodes in the network"
              }

def base_conf():
    network   = "medium"
    traffic   = "4_5"
    mobility  = "20"
    blackhole = "0.0"
    return network, traffic, mobility, blackhole

def file_name(
              work_dir, 
              network=base_conf()[0], 
              traffic=base_conf()[1], 
              mobility=base_conf()[2], 
              blackhole=base_conf()[3]):
    return work_dir + network + "-" + traffic + "-" + mobility + "-" + blackhole

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
            " --blackholes=" + blackhole +
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
    all_cmds[:] = [x for x in all_cmds if not x in default_cmd]
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
    if os.path.exists(fileprefix):
        os.remove(fileprefix)
    for click in clicks:
        # Read out metrics from all runs
        accum = numpy.array([])
        for i in runs:
            current_file_name = fileprefix + "-" + click + "-" + `i`
            if not os.path.exists(current_file_name):
                continue
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
        
        if not result:
            continue
        
        # Write results for this Click configuration
        out_file = file(fileprefix, "a")
        out_file.write(click)
        for x in result:
            out_file.write(" " + str(x))
        out_file.write("\n")
        out_file.close()
        
        accum_file = file(fileprefix + '-' + click, "w")
        for line in accum:
            for val in line:
                accum_file.write(" " + str(val))
            accum_file.write("\n")
        accum_file.close()

def write_list_to_file(file_name, mylist):
    out_file = file(file_name, "w")
    for line in mylist:
        for entry in line:
            out_file.write(entry + ' ')
        out_file.write('\n')
    out_file.close()

def generate_plots(work_dir, setting, networks=[base_conf()[0]], traffics=[base_conf()[1]], mobilities=[base_conf()[2]], blackholes=[base_conf()[3]], clicks=clicks):
    
    pdr = [['title']]
    for click in clicks:
        pdr[0].extend([dictionary[click], dictionary[click]])
    bu = [['title']]
    for click in clicks:
        bu[0].extend([dictionary[click], dictionary[click]])
    delay = [['title']]
    for click in clicks:
        delay[0].extend([dictionary[click], dictionary[click]])
        
    num_settings = 0
    
    for network in networks:
        for traffic in traffics:
            for mobility in mobilities:
                for blackhole in blackholes:
                    name = file_name(work_dir, network, traffic, mobility, blackhole)
                    if not os.path.exists(name):
                        continue
                    in_file = file(name, "r")
                    entryname = os.path.basename(name)
                    if len(networks) > 1:
                        entryname = network
                    elif len(traffics) > 1:
                        entryname = traffic
                    elif len(mobilities) > 1:
                        entryname = mobility
                    elif len(blackholes) > 1:
                        entryname = blackhole
                    entryname = dictionary[entryname] if not dictionary[entryname] == None else entryname
                    pdr_entry = [entryname]
                    bu_entry = [entryname]
                    delay_entry = [entryname]
                    for line in in_file.readlines():
                        split_line = line.split()
                        pdr_entry.extend(split_line[1:3])
                        bu_entry.extend(split_line[3:5])
                        delay_entry.extend(split_line[11:13])
                    pdr.append(pdr_entry)
                    bu.append(bu_entry)
                    delay.append(delay_entry)
                    num_settings += 1
    
    if num_settings == 0:
        return
    
    pdr_file = work_dir + setting + "-pdr.dat"
    bu_file = work_dir + setting + "-bu.dat"
    delay_file = work_dir + setting + "-delay.dat"
    
    write_list_to_file(pdr_file, pdr)
    write_list_to_file(bu_file, bu)
    write_list_to_file(delay_file, delay)
    
    common_params = "setting='" + dictionary[setting] + "';" + "minx='" + `-0.5` + "';" + "maxx='" + `num_settings - 0.5` + "';" + "tikz='true'"
    gnu_script = "plot.gnu"
    
    subprocess.call(["gnuplot", "-e",
                     "filename='" + pdr_file + "';" + 
                     "metric='Packet Delivery Ratio';" + 
                     "maxy='1';" +
                     "outfile='out/" + setting + "-pdr.tikz';" +
                     common_params,
                     gnu_script])
    subprocess.call(["gnuplot", "-e", 
                     "filename='" + bu_file + "';" +
                     "metric='Bandwidth Utilization [bytes]';" +
                     "setting='" + setting + "';" +
                     "outfile='out/" + setting + "-bu.tikz';" +
                     common_params,
                     gnu_script])
    subprocess.call(["gnuplot", "-e", 
                     "filename='" + delay_file + "';" + 
                     "metric='Delay [ms]';" + 
                     "maxy='200';" +
                     "setting='" + setting + "';" + 
                     "outfile='out/" + setting + "-delay.tikz';" +
                     common_params,
                     gnu_script])

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
    parser.add_argument("-o", "--onlyeval", help="Whether to only perform the evaluation, specify directory (obsoletes --comment option)")
    args = parser.parse_args(argv)

    work_dir = ""
    # Create workdir
    if not args.onlyeval:
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
        
    else:
        work_dir = args.onlyeval
 
    print "Evaluate experiments"
    evaluate(work_dir)
    generate_plots(work_dir, "network", networks=networks)
    generate_plots(work_dir, "traffic", traffics=traffics_groupsize)
    generate_plots(work_dir, "mobility", mobilities=mobilities)
    generate_plots(work_dir, "numgroups", traffics=traffics_numgroups)
    generate_plots(work_dir, "blackhole", blackholes=blackholes)

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
    