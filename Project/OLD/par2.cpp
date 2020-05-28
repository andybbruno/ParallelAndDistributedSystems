#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "lib/tools.cpp"
#include "lib/utimer.cpp"

void arg_check(int argc)
{
    if (argc < 5)
    {
        std::cout << "\nUsage:   #Workers   #Elements   Range   Seed (0 means random)\n\n";
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    arg_check(argc);

    int nw = atoi(argv[1]);
    int dim = atoi(argv[2]);
    int limit = atoi(argv[3]);

    if (limit == 0)
        limit = INT16_MAX;

    int seed = atoi(argv[4]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);

    std::vector<int> vec = tools::rand_vec(dim, limit);
    std::vector<std::pair<int, int>> ranges = tools::make_ranges(dim, nw);

    for (auto x : ranges)
    {
        std::cout << x.first << " - " << x.second << std::endl;
    }

    // auto begin = std::chrono::system_clock::now();

    std::vector<std::thread> tids1;
    std::vector<std::thread> tids2;

    bool sorted = false;
    while (!sorted)
    {
        std::vector<int> tmp(dim);
        sorted = true;
        for (int i = 0; i < nw; i++)
        {
            auto rng = ranges[i];
            tids1.push_back(std::thread([rng, &vec, &sorted, &tmp]() {
                for (int x = rng.first; x <= rng.second; x += 2)
                {
                    if (vec[x] > vec[x + 1])
                    {
                        tmp[x] = vec[x + 1];
                        tmp[x + 1] = vec[x];
                        sorted = false;
                    }
                    else
                    {
                        tmp[x] = vec[x];
                        tmp[x + 1] = vec[x + 1];
                    }
                }
            }));
        }

        for (std::thread &t : tids1)
        {
            t.join();
        }

        vec = tmp;
        tmp = std::vector<int>(dim);

        for (int i = 0; i < nw; i++)
        {
            auto rng = ranges[i];
            tids2.push_back(std::thread([rng, &vec, &sorted, &tmp]() {
                for (int y = rng.first + 1; y <= rng.second; y += 2)
                {
                    if (vec[y] > vec[y + 1])
                    {
                        tmp[y] = vec[y + 1];
                        tmp[y + 1] = vec[y];
                        sorted = false;
                    }
                    else
                    {
                        tmp[y] = vec[y];
                        tmp[y + 1] = vec[y + 1];
                    }
                }
            }));
        }

        for (std::thread &t : tids2)
        {
            t.join();
        }
    }

    // auto end = std::chrono::system_clock::now();
    // auto elapsed = end - begin;
    // auto musec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    // std::cout << musec << std::endl;

    tools::print(vec);

    return 0;
}