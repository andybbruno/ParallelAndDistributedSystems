#include <iostream>
#include <vector>
#include <algorithm>

void print(std::vector<std::vector<bool>> const &vec)
{
    std::cout << std::string(vec[0].size(), '*') + "\t" << std::endl
              << std::endl;

    for (std::vector<bool> x : vec)
    {
        for (bool b : x)
        {
            std::cout << (b == true ? '*' : ' ') << "\t";
        }
        std::cout << std::endl;
    }

    std::cout << std::string(vec[0].size(), '*') << std::endl
              << std::endl;
}

int main(int argc, char *argv[])
{
    size_t niter = atoi(argv[1]);
    size_t seed = atoi(argv[2]);
    size_t n = atoi(argv[3]);
    size_t m = atoi(argv[4]);
    size_t nw = atoi(argv[5]);

    srand(seed);

    std::vector<std::vector<bool>> matrix(n, std::vector<bool>(m, false));

    size_t offset = 1;
    for (size_t i = 1; i < n - 1; i += offset)
    {
        for (size_t j = 1; j < m - 1; j += offset)
        {
            matrix[i][j] = (rand() % 2) == 0 ? false : true;
        }
    }

    print(matrix);
    return 0;
}