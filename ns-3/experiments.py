import multiprocessing
import subprocess
import operator
import os

# Settings: possible values are defined in 'castor-xcast.cc'
runs       = range(1,21) # 20 runs
duration   = 60.0 * 10.0 # 10 min
clicks     = ["xcast", "regular"]
networks   = ["large"]#, "small"]#, "large"]
traffics   = ["20_1", "10_2", "5_5"]
mobilities = ["10"]

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
    out_file.write("")
    out_file.close()
    for click in clicks:
        accum = []
        for i in runs:
            current_file_name = fileprefix + "-" + click + "-" + `i`
            current_file = file(current_file_name, "r")
            line = map(num, current_file.readline().split(" "))
            current_file.close()
            #os.remove(current_file_name)
            if accum:
                accum = map(operator.add, accum, line)
            else:
                accum = line
        avg = [x/len(runs) for x in accum]
        out_file = file(fileprefix, "a")
        out_file.write(click)
        for x in avg:
            out_file.write(" " + str(x))
        out_file.write("\n")
        out_file.close()
    
def num(s):
    try:
        return int(s)
    except ValueError:
        return float(s)

if __name__ == '__main__':
    print "Start experiments on " + `multiprocessing.cpu_count()` + " core(s)"
    pool = multiprocessing.Pool(None) # use 'multiprocessing.cpu_count()' cores
    pool.map_async(subprocess.call, generate_cmd())
    pool.close()
    pool.join()
    
    print "Evaluate experiments"
    evaluate()
    