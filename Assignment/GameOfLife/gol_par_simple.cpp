#include <iostream>
#include <vector>
#include <thread>
#include "lib/tools.hpp"
#include "lib/utimer.cpp"

void compute(std::vector<std::vector<bool>> const &mat, std::vector<std::vector<bool>> &res, uint64_t start, uint64_t end)
{
    auto rows = mat.size() - 2;
    auto cols = mat[0].size() - 2;

    for (size_t k = start; k < end; k++)
    {
        auto ix = (k / rows) + 1;
        auto jx = (k % cols) + 1;

        auto neig = tools::neighbourhood(mat, ix, jx);
        bool alive = mat[ix][jx];

        if (alive)
        {
            if ((neig < 2) | (neig > 3))
            {
                alive = false;
            }
            else if ((neig == 2) | (neig == 3))
            {
                alive = true;
            }
        }
        else
        {
            if (neig == 3)
            {
                alive = true;
            }
        }
        res[ix][jx] = alive;
    }
}

int main(int argc, char *argv[])
{
    size_t niter = atoi(argv[1]);

    size_t seed = atoi(argv[2]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);

    size_t n = atoi(argv[3]);
    size_t m = atoi(argv[4]);
    size_t nw = atoi(argv[5]);

    std::vector<std::thread> workers(nw);

    std::vector<std::vector<bool>> matrix(n, std::vector<bool>(m, false));
    tools::randomize(matrix);
    std::vector<std::vector<bool>> result(matrix);

    utimer u("Parallel");

    // start thread pool
    for (int i = 0; i < nw; i++)
    {
        auto start = i * m;
        auto end = (i + 1) * m;
        workers[i] = std::thread(compute, matrix, result, start, end);
    }

    for (size_t k = 0; k < niter; k++)
    {
        // compute(matrix, result, 0, 100);

        matrix = result;

        tools::print(matrix);
        tools::delay(100);
    }

    for (int i = 0; i < nw; i++)
        workers[i].join();
    return 0;
}