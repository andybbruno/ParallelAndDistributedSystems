#!/bin/bash

# echo ""
# echo ""
# echo "Start: FARM with VECTORS (EXECUTION TIME)"
# echo ""
# echo ""
# echo ""
# for nw in {1..10}
#     do
#     ./farm_vectors_exec.o $nw 100000 0 1 >> res/farm_vectors_exec.txt
# done
# echo ""
# echo ""
# echo ""
# echo "Finished!"


echo ""
echo ""
echo "Start: FARM with VECTORS (COMMUNICATION TIME)"
echo ""
echo ""
echo ""
for nw in {1..10}
    do
    ./farm_vectors_com.o $nw 100000 0 1 >> res/farm_vectors_com.txt
done
echo ""
echo ""
echo ""
echo "Finished!"