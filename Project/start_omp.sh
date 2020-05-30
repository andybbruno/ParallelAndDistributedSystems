#!/bin/bash

echo ""
echo ""
echo "Start: OMP"
echo ""
echo ""
echo ""
for nw in {2..50}
    do
    ./omp_main.o $nw 100000 0 1 >> res/omp.txt
done
echo ""
echo ""
echo ""
echo "Finished!"