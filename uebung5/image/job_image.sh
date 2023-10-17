#!/bin/bash
#SBATCH --partition wr14         # partition (queue)
#SBATCH --nodes=1                # number of nodes
#SBATCH --ntasks-per-node=56     # number of cores per node
#SBATCH --mem 10G                # memory per node in MB (different units with suffix K|M|G|T)
#SBATCH --time 20:00             # total runtime of job allocation (format D-HH:MM:SS)
#SBATCH --output slurm.%N.%j.out # STDOUT (%N: nodename, %j: job-ID)
#SBATCH --error slurm.%N.%j.err  # STDERR

module load pgi libFHBRS

echo "Programm wird ausgeführt auf " `hostname`
# Achtung: dies ist noetig wegen eines groesseren Stacks!!
ulimit -s 128000

# ausführen ohne Grafikausgabe
make run2

exit

