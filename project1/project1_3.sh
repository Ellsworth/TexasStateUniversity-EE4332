#!/bin/bash -l

#SBATCH -p shared
#SBATCH -n 1
#SBATCH -t 06:00:00
#SBATCH -J hpc-project1_3

echo "Starting at `date`"
echo "Running on hosts: $SLURM_NODELIST"
echo "Running on $SLURM_NNODES nodes."
echo "Running on $SLURM_NPROCS processors."
echo "Current working directory is `pwd`"

echo ""

module load gcc
module list

echo ""

g++ -v

echo ""

cd /home/wqi7/TexasStateUniversity-EE4332/project1

g++ project1_3.cpp -o project1_3

./project1_3