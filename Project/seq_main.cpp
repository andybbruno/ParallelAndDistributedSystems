#include <iostream>
#include <vector>
#include "lib/tools.cpp"
#include "lib/utimer.cpp"

void odd_even_sort(std::vector<int> &arr)
{
    bool sorted = false;
    auto n = arr.size();

    while (!sorted)
    {
        sorted = true;

        for (int i = 0; i <= n - 2; i += 2)
        {
            if (arr[i] > arr[i + 1])
            {
                std::swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }

        for (int i = 1; i <= n - 2; i += 2)
        {
            if (arr[i] > arr[i + 1])
            {
                std::swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }
    }
}

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

    std::vector<int> vec = tools::rand_vec(dim, limit);

    auto begin = std::chrono::system_clock::now();
    odd_even_sort(vec);
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - begin;
    auto musec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    std::cout << musec << std::endl;
    // tools::print(vec);

    return 0;
}
