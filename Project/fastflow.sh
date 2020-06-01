#!/bin/bash

echo ""
echo ""
echo "Start: FastFlow"
echo ""
echo ""
echo ""
for nw in {1..50}
    do
    ./fastflow.o $nw 100000 0 1  >> res/fastflow.txt
done
echo ""
echo ""
echo ""
echo "Finished!"