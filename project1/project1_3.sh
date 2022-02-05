#!/bin/bash -l
	 
#SBATCH -p shared 
#SBATCH -n 1  
#SBATCH -t 06:00:00
#SBATCH -J hpc-project1_3

module load gcc
module list

g++ -v

cd /home/wqi7/TexasStateUniversity-EE4332/project1 

g++ project1_3.cpp -o project1_3

./project1_3
