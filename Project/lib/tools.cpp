#pragma once
#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <numeric>

namespace tools
{
    std::vector<std::pair<uint, uint>> make_ranges(uint dim, uint nw, uint step_size = 1, uint odd = false)
    {
        std::vector<std::pair<uint, uint>> ranges;
        std::vector<std::vector<uint>> item_list(nw);
        std::vector<uint> idx;

        for (int i = 0; i < dim; i++)
        {
            if (i % 2 == odd)
            {
                idx.push_back(i);
            }
        }
        uint bucket_size = idx.size() / nw;
        uint mod = idx.size() % nw;
        uint a;
        uint b;
        for (int i = 0; i < nw; i++)
        {
            for (int k = 0; k < bucket_size; k++)
            {
                auto index = idx.front();
                idx.erase(idx.begin());
                item_list[i].push_back(index);
            }
            if (mod > 0)
            {
                auto index = idx.front();
                idx.erase(idx.begin());
                item_list[i].push_back(index);
                --mod;
            }
        }

        for (auto list : item_list)
        {
            ranges.push_back(std::make_pair(list.front(), list.back()));
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