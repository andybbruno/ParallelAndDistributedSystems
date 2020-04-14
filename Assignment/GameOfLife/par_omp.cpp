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

    std::vector<std::vector<bool>> table(n, std::vector<bool>(m, false));

    tools::randomize(table);

    // create_glider(table, 1, 1);
    // create_glider(table, 1, 10);

    if (doprint)
    {
        tools::print(table);
    }

    utimer u("Sequential");

    for (size_t k = 0; k < niter; k++)
    {
        std::vector<std::vector<bool>> res_table(n, std::vector<bool>(m, false));

#pragma omp parallel num_threads(nw)
        {
            for (size_t i = 1; i < n - 1; i++)
            {
                for (size_t j = 1; j < m - 1; j++)
                {
                    int neig = tools::neighbours(table, i, j);
                    bool alive = table[i][j];

                    if (alive)
                    {
                        if ((neig < 2) | (neig > 3))
                        {
                            res_table[i][j] = false;
                        }
                        else if ((neig == 2) | (neig == 3))
                        {
                            res_table[i][j] = true;
                        }
                    }
                    else
                    {
                        if (neig == 3)
                        {
                            res_table[i][j] = true;
                        }
                    }
                }
            }
        }
        table = std::move(res_table);

        // tools::print(table);
        // tools::delay(30);
    }

    if (doprint)
    {
        tools::print(table);
    }
    return 0;
}