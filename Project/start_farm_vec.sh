#!/bin/bash

echo ""
echo ""
echo "Start: FARM WITH VECTORS"
echo ""
echo ""
echo ""
for nw in {2..32}
    do
    ./farm_vec_main.o $nw 100000 0 1 >> res/farm_vec.txt
done
echo ""
echo ""
echo ""
echo "Finished!"