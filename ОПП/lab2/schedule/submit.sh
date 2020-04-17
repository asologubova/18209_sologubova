#!/bin/bash

#PBS -l walltime=00:05:00
#PBS -l select=1:ncpus=4:ompthreads=4
#PBS -q S1995178

cd $PBS_O_WORKDIR
source /opt/intel/composerxe/bin/compilervars.sh intel64

echo num of threads $OMP_NUM_THREADS
echo
echo without
./without
echo static
./static
echo dynamic
./dynamic
echo guided
./guided
echo runtime
./runtime