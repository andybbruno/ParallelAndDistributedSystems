#!/bin/bash

rm -rf res
mkdir res
echo ""
echo ""
echo "Start: FARM"
echo ""
echo ""
echo ""
for nw in 1 2 4 8 16 32 64 128 254
    do
    ./farm_main.o $nw 100000 999999 1
done
echo ""
echo ""
echo ""
echo "Finished!"