#!/bin/bash
#SBATCH --partition wr44         # partition (queue)
#SBATCH --nodes=1                # number of nodes
#SBATCH --ntasks-per-node=256    # number of cores per node
#SBATCH --mem 30G                # memory per node in MB (different units with suffix K|M|G|T)
#SBATCH --time 1:00:00           # total runtime of job allocation (format D-HH:MM:SS)
#SBATCH --output slurm.%N.%j.out # STDOUT (%N: nodename, %j: job-ID)
#SBATCH --error slurm.%N.%j.err  # STDERR

# start from scratch, remove old executable
module load gcc libFHBRS

# compile and start a small test run for correctness
make test

# make timings
make run

exit
