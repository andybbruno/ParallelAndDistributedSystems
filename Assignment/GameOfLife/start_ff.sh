#!/bin/bash


iterations='100'
seed='1'

echo "Compiling.."
g++ -std=c++17  -g  -I spm2020user/fastflow  -O3 -finline-functions -DNDEBUG -o par_ff.o par_ff.cpp -pthread


echo ""
echo ""
echo "Start: FastFlow"
for nw in 1 4 16 64 256
    do for i in 256 1024 4096 16384
        do 
        echo -e "NW =" $nw '\t' "N = M =" $i
        for j in {1..5} 
            do ./par_ff.o $iterations $seed $i $i $nw>> results/par_ff.csv
        done
    done 
done
echo ""
echo ""
echo ""
echo "Finished!"