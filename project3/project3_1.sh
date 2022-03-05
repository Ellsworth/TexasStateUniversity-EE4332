#!/bin/bash -l

#SBATCH -p parallel
#SBATCH -n 28
#SBATCH -t 01:00:00
#SBATCH -J hpc-project3_1


echo "Starting at `date`"
echo "Running on hosts: $SLURM_NODELIST"
echo "Running on $SLURM_NNODES nodes."
echo "Running on $SLURM_NPROCS processors."
echo "Current working directory is `pwd`"

echo ""

module load intel
module list

echo ""

icpc -v

echo ""

cd /home/wqi7/TexasStateUniversity-EE4332/project3

icpc -O3 -o project3_1 -xAVX2 project3_1.cpp

./project3_1

rm project3_1

echo "Finished at `date`."
