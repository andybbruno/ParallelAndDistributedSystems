#!/bin/bash

rm -rf res
mkdir res
echo ""
echo ""
echo "Start: MASTER-WORKER"
echo ""
echo ""
echo ""
for nw in 1 2 5 10 15 30 50 100 150 200
    do
    ./mawo_main.o $nw 100000 999999 1
done
echo ""
echo ""
echo ""
echo "Finished!"