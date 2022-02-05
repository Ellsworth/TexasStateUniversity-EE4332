#!/bin/bash -l

#SBATCH -p shared
#SBATCH -n 1
#SBATCH -t 00:00:10
#SBATCH -J hpc-project1_2

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

g++ project1_2.cpp -o project1_2

./project1_2

rm project1_2