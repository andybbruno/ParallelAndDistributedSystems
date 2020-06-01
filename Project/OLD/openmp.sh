#!/bin/bash

echo ""
echo ""
echo "Start: OMP"
echo ""
echo ""
echo ""
for nw in {1..50}
    do
    ./openmp.o $nw 100000 0 1 >> res/openmp.txt
done
echo ""
echo ""
echo ""
echo "Finished!"