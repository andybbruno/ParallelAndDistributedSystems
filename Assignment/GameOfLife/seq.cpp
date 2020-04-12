#include <iostream>
#include <vector>
#include "lib/tools.hpp"
#include "lib/utimer.cpp"

int main(int argc, char *argv[])
{
    size_t niter = atoi(argv[1]);

    size_t seed = atoi(argv[2]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);

    size_t n = atoi(argv[3]);
    size_t m = atoi(argv[4]);

    std::vector<std::vector<bool>> matrix(n, std::vector<bool>(m, false));

    tools::randomize(matrix);

    // create_glider(matrix, 1, 1);
    // create_glider(matrix, 1, 10);

    // tools::print(matrix);

    utimer u("Sequential");

    for (size_t k = 0; k < niter; k++)
    {
        std::vector<std::vector<bool>> matrix_new(n, std::vector<bool>(m, false));

        for (size_t i = 1; i < n - 1; i++)
        {
            for (size_t j = 1; j < m - 1; j++)
            {
                int neig = tools::neighbours(matrix, i, j);
                bool alive = matrix[i][j];

                if (alive)
                {
                    if ((neig < 2) | (neig > 3))
                    {
                        matrix_new[i][j] = false;
                    }
                    else if ((neig == 2) | (neig == 3))
                    {
                        matrix_new[i][j] = true;
                    }
                }
                else
                {
                    if (neig == 3)
                    {
                        matrix_new[i][j] = true;
                    }
                }
            }
        }

        matrix = std::move(matrix_new);
        // tools::print(matrix);
        // tools::delay(30);
    }

    // tools::print(matrix);
    return 0;
}