#include <iostream>
#include <future>
#include <vector>
#include <algorithm>
#include "utimer.cpp"
#include "base_async.cpp"

bool basecase(std::vector<int> &v)
{
    return v.size() == 1;
}

std::vector<int> solve(std::vector<int> &vec)
{
    return vec;
}

std::vector<std::vector<int>> divide(std::vector<int> &v)
{
    std::vector<std::vector<int>> res;

    std::size_t const half = v.size() / 2;

    std::vector<int> lo;
    for (int i = 0; i < half; i++)
    {
        lo.push_back(v[i]);
    }

    std::vector<int> hi;
    for (int i = half; i < v.size(); i++)
    {
        hi.push_back(v[i]);
    }

    res.push_back(lo);
    res.push_back(hi);

    return res;
}

std::vector<int> conquer(std::vector<std::vector<int>> &chunks)
{
    std::vector<int> res;

    auto v1 = chunks[0];
    auto v2 = chunks[1];
    std::merge(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(res));

    return res;
}

int main(int argc, char *argv[])
{
    utimer u("Parallel");

    uint n = atoi(argv[1]);
    bool debug = argc > 2;

    std::vector<int> v;

    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        v.push_back(rand() % 1000);
    }

    if (debug)
    {
        for (auto i : v)
        {
            std::cout << i << ".";
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    auto res = dc(v, basecase, solve, divide, conquer);

    if (debug)
    {
        for (auto i : res)
        {
            std::cout << i << ".";
        }
        std::cout << std::endl;
    }
}