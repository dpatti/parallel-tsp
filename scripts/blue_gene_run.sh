#!/bin/sh

BGLMPI_MAPPING=TXYZ
INDEX=0

for i in $@; do
    for graph in 1024; do
        for ants in 128; do
            for iter in 500; do
                mpirun \
                    -mode VN \
                    -np $i \
                    -cwd /gpfs/small/PCP2/home/PCP2patt/project \
                    ./aco --graph_size $graph --ants $ants --iterations $iter \
                     > ./out/stdout-$i-$graph-$ants-$iter-$SLURM_JOB_ID \
                    2> ./out/stderr-$i-$graph-$ants-$iter-$SLURM_JOB_ID
            done
        done
    done
done
