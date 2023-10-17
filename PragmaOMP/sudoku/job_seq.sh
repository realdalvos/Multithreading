#!/bin/bash
#SBATCH --partition=wr44         # partition (queue)
#SBATCH --ntasks=1
#SBATCH --mem=100G               # memory per node in MB (different units with suffix K|M|G|T)
#SBATCH --time=72:00:00          # total runtime of job allocation (format D-HH:MM:SS)
#SBATCH --output=slurm.%N.%j.out # STDOUT
#SBATCH --error=slurm.%N.%j.err  # STDERR

module load intel-compiler libFHBRS

# start program
./sudoku_seq.exe 16
