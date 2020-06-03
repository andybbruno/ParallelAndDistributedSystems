#!/bin/bash
echo ""
echo ""
echo "Start: FastFlow FARM"
echo ""
echo ""
echo ""
for nw in {1..50}
    do
    ./ff_farm_tasks.o $nw 100000 0 1  >> res/ff_farm.txt
done
echo ""
echo ""
echo ""
echo "Finished!"


echo ""
echo ""
echo "Start: FastFlow MASTER-WORKER"
echo ""
echo ""
echo ""
for nw in {1..50}
    do
    ./ff_master_worker.o $nw 100000 0 1  >> res/ff_master.txt
done
echo ""
echo ""
echo ""
echo "Finished!"