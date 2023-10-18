#!/bin/bash
#SBATCH --partition=hpc3         # partition (queue)
#SBATCH --nodes=16               # number of tasks/cores
#SBATCH --ntasks-per-node=64     # number of cores per node
#SBATCH --mem=100G               # memory per node in MB (different units with suffix K|M|G|T)
#SBATCH --time=3:00:00           # total runtime of job allocation (format D-HH:MM)
#SBATCH --output=slurm.%j.out    # STDOUT (%N: nodename, %j: job-ID)
#SBATCH --error=slurm.%j.err     # STDERR

# system mpi module used here
module load gcc openmpi/gnu libFHBRS

# log2 of array size
N=30

for p in 1 2 4 8 16 32 64 128 256 512 1024; do
    mpirun -np $p ./sort.exe $N
done

echo job finished
