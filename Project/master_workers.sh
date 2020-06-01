#!/bin/bash

echo ""
echo ""
echo "Start: MASTER-WORKER"
echo ""
echo ""
echo ""
for nw in {1..50}
    do
    ./master_workers.o $nw 100000 0 1 >> res/master_workers.txt
done
echo ""
echo ""
echo ""
echo "Finished!"