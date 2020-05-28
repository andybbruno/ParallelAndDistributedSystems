#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

#include "lib/tools.cpp"

void OddEvenSort(std::vector<int> &A, int nw)
{

    bool swap = true;
    int N = A.size();

    while (swap)
    {
        swap = false;
#pragma omp parallel num_threads(nw)
        {
#pragma omp for
            for (int i = 0; i < N - 1; i += 2)
            {
                if (A[i] > A[i + 1])
                {
                    std::swap(A[i], A[i + 1]);
                    swap = true;
                }
            }
#pragma omp for
            for (int i = 1; i < N - 1; i += 2)
            {
                if (A[i] > A[i + 1])
                {
                    std::swap(A[i], A[i + 1]);
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
    tools::print(vec);
    auto begin = std::chrono::system_clock::now();
    OddEvenSort(vec, nw);
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - begin;
    auto musec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    std::cout << musec << std::endl;
    tools::print(vec);
}