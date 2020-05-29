#include <iostream>
#include <vector>
#include <ff/utils.hpp>
#include <ff/parallel_for.hpp>
#include "lib/ff_tools.cpp"
#include "lib/utimer.cpp"

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cout << "\nUsage:   #Workers   #Elements   Range   Seed (0 means random)\n\n";
        exit(EXIT_FAILURE);
    }

    int nw = atoi(argv[1]);
    int dim = atoi(argv[2]);
    int range = atoi(argv[3]);
    if (range == 0)
        range = INT16_MAX;
    int seed = atoi(argv[4]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);

    if ((nw >= dim))
    {
        std::cout << "\n Too many workers! \n\n";
        exit(EXIT_FAILURE);
    }
    if (range < 0)
    {
        std::cout << "\n Range must be either positive or '0' for random! \n\n";
        exit(EXIT_FAILURE);
    }

    std::vector<int> vec = tools::rand_vec(dim, range);

    utimer u(std::to_string(nw) + "," + std::to_string(dim));
    ff::ParallelFor pf(nw);

    bool swap = true;
    bool even = true;
    while (swap)
    {
        swap = false;
        pf.parallel_for(even, dim - 1, 2, [&](const long i) {
            {
                if (vec[i] > vec[i + 1])
                {
                    std::swap(vec[i], vec[i + 1]);
                    swap = true;
                }
            }
        });
        even = !even;
    }
    assert(std::is_sorted(vec.begin(), vec.end()));
    return 0;
}