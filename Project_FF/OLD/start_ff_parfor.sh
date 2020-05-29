#!/bin/bash

rm -rf res
mkdir res
echo ""
echo ""
echo "Start: FastFlow ParFor"
echo ""
echo ""
echo ""
for nw in 1 2 5 10 15 30 50 100 150 200
    do
    ./ff_parfor.o $nw 100000 999999 1
done
echo ""
echo ""
echo ""
echo "Finished!"