#!/bin/sh
 
#PBS -l walltime=00:03:00
#PBS -l select=2:ncpus=8:mpiprocs=8:mem=4000m
 
cd $PBS_O_WORKDIR
 
## Set variables for ITAC:
source /opt/intel/itac/8.1.3.037/bin/itacvars.sh
 
## Set variables for Intel compiler:
source /opt/intel/composerxe/bin/compilervars.sh intel64
export I_MPI_CC=icc
 
## Compile with '-trace' parameter to use ITAC:
mpicc -trace -O3 lab3.c -o lab3_16
 
## Count the number of MPI processes:
MPI_NP=`wc -l $PBS_NODEFILE | awk '{ print $1 }'`
 
## Add '-trace' parameter:
mpirun -trace -machinefile $PBS_NODEFILE -np $MPI_NP ./lab3_16