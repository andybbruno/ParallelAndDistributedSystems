#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

#include "lib/tools.cpp"

void OddEvenSort(std::vector<int> &A, int nw)
{
    int exch = 1, start = 0, i;
    int temp;
    int N = A.size();
    
    while (exch || start)
    {
        exch = 0;
#pragma omp parallel for private(temp) shared(start, exch) num_threads(nw)
        for (i = start; i < N - 1; i += 2)
        {
            if (A[i] > A[i + 1])
            {
                temp = A[i];
                A[i] = A[i + 1];
                A[i + 1] = temp;
                exch = 1;
            }
        }
        if (start == 0)
            start = 1;
        else
            start = 0;
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
    auto begin = std::chrono::system_clock::now();
    OddEvenSort(vec, nw);
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - begin;
    auto musec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    std::cout << musec << std::endl;
}