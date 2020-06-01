#!/bin/bash

echo ""
echo ""
echo "Start: FARM"
echo ""
echo ""
echo ""
for nw in {1..50}
    do
    ./farm_tasks.o $nw 100000 0 1 >> res/farm_tasks.txt
done
echo ""
echo ""
echo ""
echo "Finished!"