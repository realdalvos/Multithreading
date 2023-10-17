#!/bin/bash
#SBATCH --partition=wr44         # partition (queue)
#SBATCH --ntasks=256
#SBATCH --mem=100G               # memory per node in MB (different units with suffix K|M|G|T)
#SBATCH --time=72:00:00          # total runtime of job allocation (format D-HH:MM:SS)
#SBATCH --output=slurm.%N.%j.out # STDOUT
#SBATCH --error=slurm.%N.%j.err  # STDERR

module load intel-compiler libFHBRS

# start program
for p in 1 2 4 8 16 32 64 128 256; do
    echo "running with " $p " threads"
    export OMP_NUM_THREADS=$p
    ./sudoku_par.exe 16
done
