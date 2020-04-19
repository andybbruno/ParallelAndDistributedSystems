#!/bin/bash

rm -f results/pfor.csv
rm -f results/ofarm.csv
rm -f results/set.csv
rm -f results/seq.csv

a='2'
b='10000000' 

# echo "Compiling.."
# make all


echo ""
echo ""
echo "Start: Seq"
for j in {1..5} 
    do ./primes.o $a $b >> results/seq.csv
done


echo ""
echo ""
echo "Start: OFarm"
for nw in 1 4 16 64 256
    do echo -e "NW =" $nw
    for j in {1..5} 
        do ./master_worker_OFarm.o $a $b $nw >> results/ofarm.csv
    done
done 

echo ""
echo ""
echo "Start: Set"
for nw in 1 4 16 64 256
    do echo -e "NW =" $nw
    for j in {1..5} 
        do ./master_worker_Set.o $a $b $nw >> results/set.csv
    done
done

echo ""
echo ""
echo "Start: ParallelFor"

for nw in 1 4 16 64 256
    do 
    for sched in -10 0 10
    do echo -e "NW =" $nw '\t' "sched=" $sched
        for j in {1..5} 
            do ./pfor_ffprimes.o $a $b $nw $sched >> results/pfor.csv
        done
    done
done

echo ""
echo ""
echo ""
echo "Finished!"