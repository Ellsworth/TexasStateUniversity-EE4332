#!/bin/bash -l
	 
#SBATCH -p shared 
#SBATCH -n 1  
#SBATCH -t 00:00:10    
#SBATCH -J hpc-project1_1

module load gcc
module list

g++ -v

cd /home/wqi7/TexasStateUniversity-EE4332/project1	 

g++ project1_1.cpp -o project1_1

./project1_1
