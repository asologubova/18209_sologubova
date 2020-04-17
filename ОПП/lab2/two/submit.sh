#!/bin/bash

#PBS -l select=1:ncpus=8
#PBS -l walltime=00:02:00
#PBS -q S1995178

cd $PBS_O_WORKDIR
source /opt/intel/composerxe/bin/compilervars.sh intel64

for i in 1 2 4 8 12
do
        export OMP_NUM_THREADS=$i
	echo num of threads $OMP_NUM_THREADS
	./a.out
        echo ----------------
done

