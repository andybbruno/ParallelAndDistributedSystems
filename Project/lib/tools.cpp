#pragma once
#include <vector>
#include <iostream>

namespace tools
{
    std::vector<std::pair<uint, uint>> make_ranges(int dim, int nw)
    {
        std::vector<std::pair<uint, uint>> ranges(nw);
        int delta = dim / nw;
        int mod = dim % nw;
        for (int i = 0; i < nw; i++)
        {
            uint a = (i != 0 ? ranges[i - 1].second + 1 : 0);
            uint b = (i - mod < 0 ? a + delta : a + delta - 1);
            ranges[i] = std::make_pair(a, b);
        }
        return ranges;
    }

    std::vector<int> rand_vec(size_t n, int limit)
    {
        std::vector<int> vec;
        for (int i = 1; i <= n; i++)
        {
            vec.push_back((i * rand()) % limit);
        }
        return vec;
    }

    void print(std::vector<int> &vec)
    {
        std::cout << "\n";
        for (auto a : vec)
        {
            std::cout << a << "\t";
        }
        std::cout << "\n";
        std::cout << "\n";
    }
} // namespace tools