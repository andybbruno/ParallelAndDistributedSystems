#!/bin/bash

echo ""
echo ""
echo "Start: FastFlow"
echo ""
echo ""
echo ""
for nw in {2..50}
    do
    ./ff.o $nw 100000 0 1  >> res/ff.txt
done
echo ""
echo ""
echo ""
echo "Finished!"