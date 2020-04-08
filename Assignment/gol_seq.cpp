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

    std::vector<std::vector<bool>> matrix;

    for (size_t i = 0; i < n; i++)
    {
        std::vector<bool> tmp(m);
        std::for_each(tmp.begin(),
                      tmp.end(),
                      [&](bool x) { return (rand() % 2) == 0 ? false : true; });
        matrix.push_back(tmp);
        
    }

    return 0;
}