#include <iostream>
#include <vector>
#include <algorithm>
#include "utils.hpp"


int main(int argc, char *argv[])
{
    size_t niter = atoi(argv[1]);

    size_t seed = atoi(argv[2]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);

    size_t n = atoi(argv[3]);
    size_t m = atoi(argv[4]);
    size_t nw = atoi(argv[5]);

    std::vector<std::vector<bool>> matrix(n, std::vector<bool>(m, false));

    matrix::randomize(matrix);

    // create_glider(matrix, 1, 1);
    // create_glider(matrix, 1, 10);

    matrix::print(matrix);

    for (size_t k = 0; k < niter; k++)
    {
        std::vector<std::vector<bool>> matrix_new(n, std::vector<bool>(m, false));

        for (size_t i = 1; i < n - 1; i++)
        {
            for (size_t j = 1; j < m - 1; j++)
            {
                int neig = game::neighbourhood(matrix, i, j);
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

        matrix = matrix_new;
        matrix::print(matrix);
        utils::delay(100);
    }

    return 0;
}