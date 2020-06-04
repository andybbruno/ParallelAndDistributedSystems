#!/bin/bash

echo ""
echo ""
echo "Start: FARM with TASKS (EXECUTION TIME)"
echo ""
echo ""
echo ""
for nw in {1..10}
    do
    ./farm_tasks_com.o $nw 100000 0 1 >> res/farm_tasks_com.txt
done
echo ""
echo ""
echo ""
echo "Finished!"


echo ""
echo ""
echo "Start: FARM with TASKS (COMMUNICATION TIME)"
echo ""
echo ""
echo ""
for nw in {1..10}
    do
    ./farm_tasks_exec.o $nw 100000 0 1 >> res/farm_tasks_exec.txt
done
echo ""
echo ""
echo ""
echo "Finished!"