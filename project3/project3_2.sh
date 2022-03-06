#!/bin/bash -l

#SBATCH --job-name=mpi_test
#SBATCH --partition=parallel
#SBATCH -n 2
#SBATCH --nodes=2
#SBATCH --time=00:05:00

echo "Starting at `date`"
echo "Running on hosts: $SLURM_NODELIST"
echo "Running on $SLURM_NNODES nodes."
echo "Running on $SLURM_NPROCS processors."
echo "Current working directory is `pwd`"

# setup modules
. /etc/profile.d/modules.sh

# Load MPI environment
module load openmpi/2.1.1

cd /home/wqi7/TexasStateUniversity-EE4332/project3

mpiCC -O3 -std=c++11 -o project3_2 project3_2.cpp

mpirun -np $SLURM_NPROCS ./project3_2

echo "Program finished with exit code $? at: `date`"

rm ./project3_2
