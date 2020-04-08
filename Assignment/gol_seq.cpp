#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, char *argv[])
{
    size_t niter = atoi(argv[1]);
    size_t seed = atoi(argv[2]);
    size_t n = atoi(argv[3]);
    size_t m = atoi(argv[4]);
    size_t nw = atoi(argv[5]);

    srand(seed);

    std::vector<std::vector<bool>> matrix(n, std::vector<bool>(m));

    size_t offset = 1;
    for (size_t i = 0; i < n; i += offset)
    {
        for (size_t j = 0; j < m; j += offset)
        {
            matrix[i][j] = (rand() % 2) == 0 ? false : true;
        }
    }

    std::cout << matrix[1][1] << std::endl;

    return 0;
}