#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include <algorithm>
#include <omp.h>
#include "lib/utimer.cpp"
#include "lib/tools.cpp"

void OddEvenSort(std::vector<int> &arr, int nw)
{
    bool swap = true;
    int dim = arr.size();

    while (swap)
    {
        swap = false;
#pragma omp parallel num_threads(nw)
        {
#pragma omp for
            for (int i = 0; i < dim - 1; i += 2)
            {
                if (arr[i] > arr[i + 1])
                {
                    std::swap(arr[i], arr[i + 1]);
                    swap = true;
                }
            }
#pragma omp for
            for (int i = 1; i < dim - 1; i += 2)
            {
                if (arr[i] > arr[i + 1])
                {
                    std::swap(arr[i], arr[i + 1]);
                    swap = true;
                }
            }
        }
    }
}
int main(int argc, char *argv[])
{
    int nw = atoi(argv[1]);
    int dim = atoi(argv[2]);
    int limit = atoi(argv[3]);

    if (limit == 0)
        limit = INT16_MAX;

    int seed = atoi(argv[4]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);

    std::vector<int> vec = tools::rand_vec(dim, limit);
    utimer u(std::to_string(nw) + "," + std::to_string(dim));
    
    OddEvenSort(vec, nw);
    
    return 0;
}