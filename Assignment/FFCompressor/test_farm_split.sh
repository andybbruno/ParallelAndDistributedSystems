for nw in 1 4 8 16 32 64 128 256
    do 
    find . -name "*.tmp.part*" -delete
    find . -name "*.zip" -delete
    ./ffc_farm_split $nw BIG/ 10000 >> res/farm_split.txt
done