#!/bin/bash

rm -rf res
mkdir res
echo ""
echo ""
echo "Start: OMP"
echo ""
echo ""
echo ""
for nw in 1 4 8 16 32 64 128 256
    do
    ./par_omp.o $nw 10000 999999 123
done
echo ""
echo ""
echo ""
echo "Finished!"