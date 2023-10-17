#!/bin/bash
#SBATCH --partition wr44         # partition (queue)
#SBATCH --nodes=1                # number of nodes
#SBATCH --ntasks-per-node=96     # number of cores per node
#SBATCH --mem 150G                # memory per node in MB (different units with suffix K|M|G|T)
#SBATCH --time 1:00:00           # total runtime of job allocation (format D-HH:MM:SS)
#SBATCH --output slurm.%N.%j.out # STDOUT (%N: nodename, %j: job-ID)
#SBATCH --error slurm.%N.%j.err  # STDERR

module load java
height=28

for nthreads in 1 2 4 8 16 24 32 48 64 96 128 192 256; do
    for level in 2 4 6 8 10 12 14 16 18; do
#       echo "binary tree of height " $height ", parallel level at " $level " using " $nthreads " threads"
        java -Xms80G -Xmx80G TreeCalculation $height $level $nthreads
    done
done

exit
