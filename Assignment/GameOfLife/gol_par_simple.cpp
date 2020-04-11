#include <iostream>
#include <vector>
#include <thread>
#include "lib/syque.hpp"
#include "lib/tools.hpp"
#include "lib/utimer.cpp"

#define EOS \
    std::pair { -1, -1 }

int main(int argc, char *argv[])
{
    uint64_t niter = atoi(argv[1]);

    size_t seed = atoi(argv[2]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);

    uint64_t n = atoi(argv[3]);
    uint64_t m = atoi(argv[4]);
    uint64_t nw = atoi(argv[5]);

    uint64_t n_ = n - 2;
    uint64_t m_ = m - 2;
    uint64_t nelem = n_ * m_;

    std::mutex m_queue;
    syque<std::pair<int, int>> queue;
    
    std::vector<std::thread> workers(nw);

    std::vector<std::vector<bool>> matrix(n, std::vector<bool>(m, false));
    tools::randomize(matrix);
    std::vector<std::vector<bool>> result(matrix);

    utimer u("Parallel");

    auto fill_queue = [&]() {
        uint64_t delta = (nelem / nw) - 1;
        uint64_t mod = nelem % nw;

        uint64_t a = 0;
        uint64_t b = 0;

        for (int i = 0; i < nw; i++)
        {
            a = (i != 0 ? b + 1 : 0);
            b = (mod > 0 ? a + delta + 1 : a + delta);
            if (mod > 0)
                --mod;
            std::pair<int, int> range{a, b};
            queue.push(range);
        }
    };

    auto compute = [&]() {
        while (true)
        {
            auto range = queue.pop();

            if (range == EOS)
                return;

            auto rows = n - 2;
            auto cols = m - 2;

            for (size_t k = range.first; k < range.second; k++)
            {
                auto ix = (k / rows) + 1;
                auto jx = (k % cols) + 1;

                auto neig = tools::neighbourhood(matrix, ix, jx);
                bool alive = matrix[ix][jx];

                if (alive)
                {
                    if ((neig < 2) | (neig > 3))
                    {
                        result[ix][jx] = false;
                    }
                    else if ((neig == 2) | (neig == 3))
                    {
                        result[ix][jx] = true;
                    }
                }
                else
                {
                    if (neig == 3)
                    {
                        result[ix][jx] = true;
                    }
                }
                // result[ix][jx] = alive;
            }

        }
    };

    tools::print(matrix);
    fill_queue();

    // start thread pool
    for (int i = 0; i < nw; i++)
        workers[i] = std::thread(compute);

    // for (size_t k = 0; k < niter; k++)
    // {
    //     // fill_queue();
    //     nelem = n_ * m_;

    //     // while (nelem > 0)
    //     // {
    //     //     continue;
    //     // }
    //     // compute(matrix, result, 0, 100);

    //     matrix = result;

    //     tools::print(matrix);
    //     // tools::delay(100);
    // }

    for (int i = 0; i < nw; i++)
        workers[i].join();
    return 0;
}