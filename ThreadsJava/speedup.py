import collections
import sys
import subprocess

if __name__ == "__main__":

    # initialize
    threadCounts = set()
    taskCounts = set()
    S = collections.defaultdict(dict)

    # process input
    for line in sys.stdin:
        splitline = line.split()
        nthreads = int(splitline[7])
        ntasks = int(splitline[10])
        speedup = float(splitline[14])
        # add threadcount and taskcount to sets
        threadCounts.add(nthreads)
        taskCounts.add(ntasks)
        # store speedup
        S[nthreads][ntasks] = speedup

    ThreadsS = sorted(list(threadCounts))
    TasksS = sorted(list(taskCounts))

    # generate output data
    f=open('speedup.dat', 'w')
    outputline = '       0'
    for t in TasksS:
        outputline += ' ' + "{0:8d}".format(t)
    f.write(outputline + '\n')
    for thr in ThreadsS:
        outputline = "{0:8d}".format(thr)
        for t in TasksS:
            outputline += ' ' + "{0:8.2f}".format(S[thr][t])
        f.write(outputline + '\n')
    f.close()

    # generate diagramm from output data
    res = subprocess.call(["gnuplot", "speedup.gnuplot"])
    print res
