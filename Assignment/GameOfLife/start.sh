#!/bin/bash

iterations='100'
seed='1'


echo "Compiling.."
make all
echo ""
echo ""
echo "Start: Sequential"
for i in 2 4 8 16 32 64 128 256 512 1024
    do 
    echo "N = M =" $i
    for j in {1..10} 
        do ./seq.o $iterations $seed $i $i >> results/seq.csv
    done
done
echo ""
echo ""
echo "Start: Parallel Active"
for nw in 2 4 8 16 32 64 128 256
    do for i in 2 4 8 16 32 64 128 256 512 1024
        do 
        echo -e "NW =" $nw '\t' "N = M =" $i
        for j in {1..10} 
            do ./par_active.o $iterations $seed $i $i $nw>> results/par_active.csv
        done
    done 
done 
echo ""
echo ""
echo "Start: Parallel Passive"
for nw in 2 4 8 16 32 64 128 256
    do for i in 2 4 8 16 32 64 128 256 512 1024
        do 
        echo -e "NW =" $nw '\t' "N = M =" $i
        for j in {1..10} 
            do ./par_passive.o $iterations $seed $i $i $nw>> results/par_passive.csv
        done
    done 
done 
echo ""
echo ""
echo "Start: Parallel OMP"
for nw in 2 4 8 16 32 64 128 256
    do for i in 2 4 8 16 32 64 128 256 512 1024
        do 
        echo -e "NW =" $nw '\t' "N = M =" $i
        for j in {1..10} 
            do ./par_omp.o $iterations $seed $i $i $nw>> results/par_omp.csv
        done
    done 
done
echo ""
echo ""
echo ""
echo "Finished!"