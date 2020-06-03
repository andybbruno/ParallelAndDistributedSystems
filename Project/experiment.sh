#!/bin/bash


echo ""
echo ""
echo "Start: SEQUENTIAL"
echo ""
echo ""
echo ""
./seq.o 100000 0 1 >> res/seq.txt
echo ""
echo ""
echo ""
echo "Finished!"


echo ""
echo ""
echo "Start: FARM"
echo ""
echo ""
echo ""
for nw in {1..50}
    do
    ./farm_tasks.o $nw 100000 0 1 >> res/farm_tasks.txt
done
echo ""
echo ""
echo ""
echo "Finished!"


echo ""
echo ""
echo "Start: FARM WITH VECTORS"
echo ""
echo ""
echo ""
for nw in {1..10}
    do
    ./farm_vectors.o $nw 100000 0 1 >> res/farm_vectors.txt
done
echo ""
echo ""
echo ""
echo "Finished!"


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


echo ""
echo ""
echo "Start: FastFlow FARM"
echo ""
echo ""
echo ""
for nw in {1..50}
    do
    ./fastflow.o $nw 100000 0 1  >> res/ff_farm.txt
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
    ./fastflow.o $nw 100000 0 1  >> res/ff_master.txt
done
echo ""
echo ""
echo ""
echo "Finished!"


echo ""
echo ""
echo "Start: OMP"
echo ""
echo ""
echo ""
for nw in {1..50}
    do
    ./openmp.o $nw 100000 0 1 >> res/openmp.txt
done
echo ""
echo ""
echo ""
echo "Finished!"