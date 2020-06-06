#pragma once
#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <numeric>

namespace tools
{   
    /**
     * @brief Creates a list of pairs. Each pair contains the start and the end of each range.
     * 
     * @param dim size of the sequence
     * @param nw number of workers
     * @param step_size the size of the jump at each iteration
     * @param odd true measn odd phase
     * @return std::vector<std::pair<uint, uint>> a list of pairs.
     */
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

    /**
     * @brief Creates a random vector 
     * 
     * @param n the size of the vector
     * @param limit the range 
     * @return std::vector<int> a vector containing random items in (-limit , limit)
     */
    std::vector<int> rand_vec(size_t n, int limit)
    {
        std::vector<int> vec;
        for (int i = 1; i <= n; i++)
        {
            vec.push_back((i * rand()) % limit);
        }
        return vec;
    }

    /**
     * @brief Print a vector of integers
     * 
     * Used during debugging
     * 
     * @param vec the input vector
     */
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