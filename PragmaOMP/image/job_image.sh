#!/bin/bash
#SBATCH --partition wr44         # partition (queue)
#SBATCH --nodes=1                # number of nodes
#SBATCH --ntasks-per-node=256    # number of cores per node
#SBATCH --mem 80G                # memory per node in MB (different units with suffix K|M|G|T)
#SBATCH --time 20:00             # total runtime of job allocation (format D-HH:MM:SS)
#SBATCH --output slurm.%N.%j.out # STDOUT (%N: nodename, %j: job-ID)
#SBATCH --error slurm.%N.%j.err  # STDERR

module load intel-compiler libFHBRS

echo "Programm wird ausgeführt auf " `hostname`
# Achtung: dies ist noetig wegen eines groesseren Stacks!!
ulimit -s 128000

# ausführen ohne Grafikausgabe
for p in 1 2 4 8 16 32 48 64 96 128 192 256; do
    export OMP_NUM_THREADS=$p
    echo "running with" $p "threads"
    make run2
done

exit

