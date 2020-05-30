#include <iostream>
#include <vector>
#include <cassert>
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
    if (argc < 4)
    {
        std::cout << "\nUsage:   #Elements   Range   Seed (0 means random)\n\n";
        exit(EXIT_FAILURE);
    }

    int dim = atoi(argv[1]);
    int range = atoi(argv[2]);
    if (range == 0)
        range = INT16_MAX;
    int seed = atoi(argv[3]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);

    if (range < 0)
    {
        std::cout << "\n Range must be either positive or '0' for random! \n\n";
        exit(EXIT_FAILURE);
    }

    std::vector<int> vec = tools::rand_vec(dim, range);

    auto begin = std::chrono::system_clock::now();
    odd_even_sort(vec);
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - begin;
    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout << musec << std::endl;
    // tools::print(vec);
    assert(std::is_sorted(vec.begin(), vec.end()));

    return 0;
}
