#!/bin/bash

rm -rf res
mkdir res
echo ""
echo ""
echo "Start: OMP"
echo ""
echo ""
echo ""
for nw in 2 5 10 20 50 100 200
    do
    ./par_omp.o $nw 100000 999999 0
done
echo ""
echo ""
echo ""
echo "Finished!"