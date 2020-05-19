#!/bin/bash

rm -rf res
mkdir res
echo ""
echo ""
echo "Start: Pool"
for nw in 1 4 8 16 32 64 128 256
    do
    ./filter_pool_parIO.o /home/bruno25-spm19/img $nw >> res_parIO.txt
done
echo ""
echo ""
echo ""
echo "Finished!"