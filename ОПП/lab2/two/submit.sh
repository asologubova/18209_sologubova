#!/bin/bash

#PBS -l select=1:ncpus=8:ompthreads=8
#PBS -l walltime=00:05:00

cd $PBS_O_WORKDIR
source /opt/intel/composerxe/bin/compilervars.sh intel64

for i in 1 2 4 8 12
do
        export OMP_NUM_THREADS = $i
        ./a.out
        echo ----------------
done

