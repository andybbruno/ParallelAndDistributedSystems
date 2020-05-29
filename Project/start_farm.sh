#!/bin/bash

echo ""
echo ""
echo "Start: FARM"
echo ""
echo ""
echo ""
for nw in 2 5 10 15 20 25 30
    do
    ./farm_main.o $nw 100000 0 1 >> res/farm.txt
done
echo ""
echo ""
echo ""
echo "Finished!"