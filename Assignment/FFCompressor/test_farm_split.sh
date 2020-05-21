for nw in 1 4 8 16 32 64 128 256
    do 
    find . -name "*.tmp.part*" -delete
    sleep 3
    find . -name "*.zip" -delete
    sleep 2
    ./ffc_farm_split $nw BIG/ 1000000 >> res/farm_split.txt
done