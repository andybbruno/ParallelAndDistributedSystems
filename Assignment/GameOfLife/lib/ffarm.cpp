#pragma once
#include "ffarm.hpp"
#include "tools.hpp"

#define EOS \
    std::pair { -1, -1 }

#define EOC -1

// Farm with Feedback
namespace FFarm
{
class Emitter : IEmitter<std::pair<int, int>>
{
private:
    // uint64_t i = 1;
    // uint64_t j = 1;
    uint64_t current = 0;
    uint64_t a = 0;
    uint64_t b = 0;
    uint64_t delta;
    uint64_t mod;
    uint64_t rows = 0;
    uint64_t cols = 0;
    uint64_t size = 0;
    uint64_t nw;

public:
    Emitter(uint64_t r, uint64_t c, uint64_t nw) : rows(r - 2), cols(c - 2), nw(nw)
    {
        size = rows * cols;
        delta = (size / nw) - 1;
        mod = size % nw;
    };

    std::pair<int, int> next()
    {
        a = (current != 0 ? b + 1 : 0);
        b = (mod > 0 ? a + delta + 1 : a + delta);
        if (mod > 0)
            --mod;

        current = b;

        return std::pair<int, int>{a, b};
    }

    bool hasNext()
    {
        return current < (size - 1);
    }

    void restart()
    {
        a = 0;
        b = 0;
        current = 0;
        mod = size % nw;
    }
};

class Worker : IWorker<std::pair<int, int>, int>
{
private:
    const std::vector<std::vector<bool>> &table;
    std::vector<std::vector<bool>> &res_table;
    uint64_t rows = 0;
    uint64_t cols = 0;

public:
    Worker(std::vector<std::vector<bool>> const &tab, std::vector<std::vector<bool>> &res) : table(tab), res_table(res)
    {
        rows = table.size() - 2;
        cols = table[0].size() - 2;
    }

    int compute(std::pair<int, int> job)
    {
        int start = job.first;
        int end = job.second;

        for (int k = start; k <= end; k++)
        {
            auto ix = (k / rows) + 1;
            auto jx = (k % cols) + 1;
            int neig = tools::neighbours(table, ix, jx);
            bool alive = table[ix][jx];

            if (alive)
            {
                if ((neig < 2) | (neig > 3))
                {
                    alive = false;
                }
                else if ((neig == 2) | (neig == 3))
                {
                    alive = true;
                }
            }
            else
            {
                if (neig == 3)
                {
                    alive = true;
                }
            }
            res_table[ix][jx] = alive;
        }
        return (end - start) + 1;
    }
};

class Collector : ICollector<int, bool>
{
private:
    uint64_t size = 0;
    uint64_t collected = 0;
    std::vector<std::vector<bool>> &table;
    std::vector<std::vector<bool>> &res_table;

public:
    Collector(std::vector<std::vector<bool>> &tab, std::vector<std::vector<bool>> &res) : table(tab), res_table(res)
    {
        size = (tab.size() - 2) * (tab[0].size() - 2);
    };

    // return true when it finishes
    bool collect(int delta)
    {
        collected += delta;

        if (collected == size)
        {
            // std::swap(table, res_table);
            table = res_table;
            collected = 0;
            return true;
        }
        return false;
    }
};

}; // namespace FFarm
