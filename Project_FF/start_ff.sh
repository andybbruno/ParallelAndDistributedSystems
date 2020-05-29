#!/bin/bash

echo ""
echo ""
echo "Start: FastFlow"
echo ""
echo ""
echo ""
for nw in 2 5 10 15 20 25 30
    do
    ./ff.o $nw 100000 0 1  >> res/ff.txt
done
echo ""
echo ""
echo ""
echo "Finished!"