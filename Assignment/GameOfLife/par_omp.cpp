#include <iostream>
#include <vector>
#include <omp.h>
#include "lib/tools.hpp"
#include "lib/utimer.cpp"

int main(int argc, char *argv[])
{
    int niter = atoi(argv[1]);
    int seed = atoi(argv[2]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);
    int n = atoi(argv[3]);
    int m = atoi(argv[4]);
    int nw = atoi(argv[5]);
    bool doprint = (argc > 6) ? true : false;

    Table table(n, Row(m, OFF));
    Table res_table(table);
    tools::randomize(table);

    // create_glider(table, 1, 1);
    // create_glider(table, 1, 10);

    if (doprint)
    {
        tools::print(table);
    }

    utimer u(std::to_string(nw) + "," + std::to_string(n));

    for (size_t k = 0; k < niter; k++)
    {
#pragma omp parallel for num_threads(nw)
        for (size_t i = 1; i < n - 1; i++)
        {
            auto prev = table[i - 1];
            auto curr = table[i];
            auto next = table[i + 1];

            auto size = curr.size();
            Row partialSum(size, 0);
            Row neigh(size, 0);
            Row res(size, 0);

            for (size_t i = 1; i < size - 1; i++)
            {
                partialSum[i] = curr[i] + prev[i] + next[i];
            }
            for (size_t i = 1; i < size - 1; i++)
            {
                neigh[i] = partialSum[i - 1] + partialSum[i] + partialSum[i + 1];
            }
            for (size_t i = 1; i < size - 1; i++)
            {
                neigh[i] -= curr[i];
            }
            for (size_t i = 1; i < size - 1; i++)
            {

                auto neig = neigh[i];
                auto alive = curr[i];

                if (alive == ON)
                {
                    if ((neig < 2) | (neig > 3))
                    {
                        res[i] = OFF;
                    }
                    else if ((neig == 2) | (neig == 3))
                    {
                        res[i] = ON;
                    }
                }
                else
                {
                    if (neig == 3)
                    {
                        res[i] = ON;
                    }
                }
            }
#pragma omp flush(res_table)
            {
                res_table[i] = res;
            }
        }
        table = res_table;
    }

    if (doprint)
    {
        tools::print(table);
    }
    return 0;
}

// for (size_t i = 1; i < n - 1; i++)
// {
//     for (size_t j = 1; j < m - 1; j++)
//     {
//         int neig = tools::neighbours(table, i, j);
//         bool alive = table[i][j];

//         if (alive)
//         {
//             if ((neig < 2) | (neig > 3))
//             {
//                 alive = false;
//             }
//             else if ((neig == 2) | (neig == 3))
//             {
//                 alive = true;
//             }
//         }
//         else
//         {
//             if (neig == 3)
//             {
//                 alive = true;
//             }
//         }
// #pragma omp flush(table, res_table)
//         res_table[i][j] = alive;
//     }
// }