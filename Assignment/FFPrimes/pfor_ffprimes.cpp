#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <mutex>
#include <ff/utils.hpp>
#include <ff/parallel_for.hpp>

using ull = unsigned long long;

static inline bool is_prime(ull n)
{
    if (n <= 3)
        return n > 1; // 1 is not prime !

    if (n % 2 == 0 || n % 3 == 0)
        return false;

    for (ull i = 5; i * i <= n; i += 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

int main(int argc, char *argv[])
{

    if (argc < 5)
    {
        std::cerr << "use: " << argv[0] << " start end nw chunksize [print=off|on]\n";
        return -1;
    }
    ull start = atoll(argv[1]);
    ull end = atoll(argv[2]);
    int nw = atoi(argv[3]);
    int chunk = atoi(argv[4]);
    bool doprint = (argc > 5) ? true : false;

    ff::ffTime(ff::START_TIME);
    ff::ParallelFor pf(nw);

    std::vector<ull> results;

    std::mutex mtx;
    pf.parallel_for(start, end, 1, chunk, [&](const long i) {
        if (is_prime(i))
        {
            mtx.lock();
            results.push_back(i);
            mtx.unlock();
        }
    });

    const size_t n = results.size();
    ff::ffTime(ff::STOP_TIME);

    if (doprint)
    {
        std::cout << "Found " << n << " primes\n";
        for (size_t i = 0; i < n; ++i)
            std::cout << results[i] << " ";
        std::cout << "\n\n";
    }
    // std::cout << "Time: " << ff::ffTime(ff::GET_TIME) << " (ms)\n";
    std::cout << nw << "," << ff::ffTime(ff::GET_TIME) << "," << chunk << "\n";
    return 0;
}