import multiprocessing
import subprocess
import sys
import numpy
import scipy.stats

# Settings: possible values are defined in 'castor-xcast.cc'
runs       = range(1,11) # 20 runs
duration   = 60.0 * 10.0 # 10 min
clicks     = ["xcast", "xcast-promisc", "regular"]
networks   = ["small"]
traffics   = ["20_1"] #, "4_5", "10_2", "20_1"]
mobilities = ["20"]

def generate_cmd():
    """Generates all commands 
    """
    return [["./waf", "--run", "castor-xcast",
            "--command-template",
            "%s --run="    + `run` +
            " --duration=" + `duration` +
            " --click="    + click +
            " --network="  + network +
            " --traffic="  + traffic +
            " --mobility=" + mobility +
            " --outfile="  + "out/" + network + "-" + traffic + "-" + mobility + "-" + click + "-" + `run`
            ]
           for run      in runs
           for click    in clicks
           for network  in networks
           for traffic  in traffics
           for mobility in mobilities
           ]

def evaluate():
    for network  in networks:
        for traffic  in traffics:
            for mobility in mobilities:
                average_runs("out/" + network + "-" + traffic + "-" + mobility, clicks)

def average_runs(fileprefix, clicks):
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
            #os.remove(current_file_name)
            if accum.any():
                accum = numpy.vstack((accum, numpy.array(line)))
            else:
                accum = numpy.array(line)
        result = []
        # Append mean and confidence interval for every metric
        for i in range(accum.shape[1]):
            m, h = mean_confidence_interval(accum[:,i])
            result.append(`m` + " " + `h`)
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
    #n = len(data)
    m, se = numpy.mean(data), scipy.stats.sem(data)
    #h = se * scipy.stats.t._ppf((1+confidence)/2., n-1) # The confidence interval
    return m, se

def main():
    print "Pre-Build experiment"
    result = subprocess.call(["./waf", "build"])
    
    if result:
        print >>sys.stderr, "Failed to build experiment, stop"
        return result
    
    print "Start experiments on " + `multiprocessing.cpu_count()` + " core(s)"
    pool = multiprocessing.Pool(None) # use 'multiprocessing.cpu_count()' cores
    pool.map_async(subprocess.call, generate_cmd())
    pool.close()
    pool.join()
    
    print "Evaluate experiments"
    evaluate()
    return 0

if __name__ == '__main__':
    sys.exit(main())
    