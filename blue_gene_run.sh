#!/bin/sh

BGLMPI_MAPPING=TXYZ
INDEX=0

for i in $@; do
	mpirun \
		-mode VN \
		-np $i \
		-cwd /gpfs/small/PCP2/home/PCP2patt/project \
		./aco --graph_size 128 --iterations 500 \
        > ./out/stdout-$i-$SLURM_JOB_ID 2> ./out/stderr-$i-$SLURM_JOB_ID
done
