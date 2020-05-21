for nw in 1 4 8 16 32 64 128 256
    do 
    find . -name "*.zip" -delete
    ./ffc_farm $nw  DATASET/ >> res/farm.txt
done