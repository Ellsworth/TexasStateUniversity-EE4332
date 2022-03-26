#!/bin/bash -l

#SBATCH -p parallel
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=28
#SBATCH -t 02:00:00
#SBATCH -J hpc-project3_2

echo "Starting at `date`"
echo "Running on hosts: $SLURM_NODELIST"
echo "Running on $SLURM_NNODES nodes."
echo "Running on $SLURM_NPROCS processors."
echo "Current working directory is `pwd`"

# setup modules
. /etc/profile.d/modules.sh

# Load MPI environment
module load openmpi/2.1.1

cd /home/wqi7/TexasStateUniversity-EE4332/project4

mpiCC -O3 -std=c++11 -fopenmp -lpthread -o project4 project4.cpp

mpirun -np 2 ./project4

rm ./project4

echo "Program finished with exit code $? at: `date`"
