#!/bin/bash -l

#SBATCH -p parallel
#SBATCH -n 28
#SBATCH -t 06:00:00
#SBATCH -J hpc-project2


echo "Starting at `date`"
echo "Running on hosts: $SLURM_NODELIST"
echo "Running on $SLURM_NNODES nodes."
echo "Running on $SLURM_NPROCS processors."
echo "Current working directory is `pwd`"

echo ""

module load intel
module load openmpi
module list

echo ""

g++ -v

echo ""

cd /home/wqi7/TexasStateUniversity-EE4332/project2

icpc -O3 -o project2 -std=c++17 -qopenmp -xAVX2 project2.cpp

./project2

rm project2

echo "Finished at `date`."
