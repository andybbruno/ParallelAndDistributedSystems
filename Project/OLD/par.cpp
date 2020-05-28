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
    // std::vector<std::pair<int, int>> ranges = tools::make_ranges(dim, nw);

    // for (auto x : ranges)
    // {
    //     std::cout << x.first << " - " << x.second << std::endl;
    // }

    bool sorted = false;
    int start = 0;

    auto begin = std::chrono::system_clock::now();
    while (!sorted)
    {
        sorted = true;
        std::vector<std::thread> tids;
        for (int i = start; i < dim - 1; i += 2)
        {
            tids.push_back(std::thread([i, &vec, &sorted]() {
                if (vec[i] > vec[i + 1])
                {
                    std::swap(vec[i], vec[i + 1]);
                    sorted = false;
                }
            }));
        }
        for (std::thread &t : tids)
        {
            t.join();
        }
        // tools::print(vec);
        start = (++start) % 2;
    }

    auto end = std::chrono::system_clock::now();
    auto elapsed = end - begin;
    auto musec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    std::cout << musec << std::endl;

    // tools::print(vec);

    return 0;
}

// std::vector<std::thread> tids1;
// std::vector<std::thread> tids2;

// bool sorted = false;
// while (!sorted)
// {
//     sorted = true;
//     for (int i = 0; i < nw; i++)
//     {
//         auto rng = ranges[i];
//         tids1.push_back(std::thread([&]() {
//             for (int x = rng.first; x < rng.second - 2; x += 2)
//             {
//                 if (vec[x] > vec[x + 1])
//                 {
//                     auto temp = vec[x];
//                     vec[x] = vec[x + 1];
//                     vec[x + 1] = temp;
//                     sorted = false;
//                 }
//             }
//         }));
//     }

//     for (std::thread &t : tids1)
//     {
//         t.join();
//     }

//     for (int i = 0; i < nw; i++)
//     {
//         auto rng = ranges[i];
//         tids2.push_back(std::thread([&]() {
//             for (int y = rng.first + 1; y < rng.second - 2; y += 2)
//             {
//                 if (vec[y] > vec[y + 1])
//                 {
//                     auto temp = vec[y];
//                     vec[y] = vec[y + 1];
//                     vec[y + 1] = temp;
//                     sorted = false;
//                 }
//             }
//         }));
//     }

//     for (std::thread &t : tids2)
//     {
//         t.join();
//     }
// }