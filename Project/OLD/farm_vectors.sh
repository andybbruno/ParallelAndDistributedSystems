#!/bin/bash

echo ""
echo ""
echo "Start: FARM WITH VECTORS"
echo ""
echo ""
echo ""
for nw in {1..10}
    do
    ./farm_vectors.o $nw 100000 0 1 >> res/farm_vectors.txt
done
echo ""
echo ""
echo ""
echo "Finished!"