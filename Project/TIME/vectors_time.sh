#!/bin/bash

echo ""
echo ""
echo "Start: FARM"
echo ""
echo ""
echo ""
for nw in 5 10 20 30 40 50
    do
    ./farm_tasks.o $nw 100000 0 1 >> res/farm_tasks.txt
done
echo ""
echo ""
echo ""
echo "Finished!"


echo ""
echo ""
echo "Start: FARM WITH VECTORS"
echo ""
echo ""
echo ""
for nw in 5 10 20 30 40 50
    do
    ./farm_vectors.o $nw 100000 0 1 >> res/farm_vectors.txt
done
echo ""
echo ""
echo ""
echo "Finished!"