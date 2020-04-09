#include <iostream>
#include <vector>
#include <thread>
#include "lib/farm.cpp"
#include "lib/tools.hpp"
#include "lib/utimer.cpp"

int main(int argc, char *argv[])
{
    size_t niter = atoi(argv[1]);
    size_t seed = atoi(argv[2]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);
    size_t n = atoi(argv[3]);
    size_t m = atoi(argv[4]);
    size_t nw = atoi(argv[5]);

    std::vector<std::vector<bool>> table(n, std::vector<bool>(m, false));
    std::vector<std::vector<bool>> res_table(n, std::vector<bool>(m, false));
    tools::randomize(table);

    Farm::Source source(n, m);
    Farm::Worker worker(table);
    Farm::Drain drain(res_table);

    tools::print(table);
    while (source.hasNext())
    {
        auto a = source.next();
        auto b = worker.compute(a);
        drain.process(b);
    }
    tools::print(res_table);
    return 0;
}