#!/bin/bash

echo ""
echo ""
echo "Start: MASTER-WORKER"
echo ""
echo ""
echo ""
for nw in {2..50}
    do
    ./mawo_main.o $nw 100000 0 1 >> res/mawo.txt
done
echo ""
echo ""
echo ""
echo "Finished!"