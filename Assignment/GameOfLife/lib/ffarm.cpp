#pragma once
#include "ffarm.hpp"
#include "tools.hpp"

#define EOS \
    std::pair { -1, -1 }

// Farm with Feedback
namespace FFarm
{
class Emitter : IEmitter<std::pair<int, int>>
{
private:
    uint64_t i = 1;
    uint64_t j = 1;
    uint64_t current = 0;

    uint64_t rows = 0;
    uint64_t cols = 0;
    uint64_t size = 0;

public:
    Emitter(uint64_t r, uint64_t c) : rows(r), cols(c), size(r * c){};

    std::pair<int, int> next()
    {
        auto pair = std::make_pair(i, j);

        ++current;
        i = (current / rows) + 1;
        j = (current % cols) + 1;

        return pair;
    }

    bool hasNext() { return current < size; }

    void restart()
    {
        current = 0;
    }
};

class Worker : IWorker<std::pair<int, int>, bool>
{
private:
    const std::vector<std::vector<bool>> &table;
    std::vector<std::vector<bool>> &res_table;

public:
    Worker(std::vector<std::vector<bool>> const &tab, std::vector<std::vector<bool>> &res) : table(tab), res_table(res) {}

    bool compute(std::pair<int, int> job)
    {
        int i = job.first;
        int j = job.second;

        int neig = tools::neighbours(table, i, j);
        bool alive = table[i][j];

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
        res_table[i][j] = alive;

        return true;
    }
};

class Collector : ICollector<bool>
{
private:
    uint64_t size = 0;
    uint64_t collected = 0;

    std::vector<std::vector<bool>> &table;
    std::vector<std::vector<bool>> &res_table;

public:
    Collector(std::vector<std::vector<bool>> &tab, std::vector<std::vector<bool>> &res) : table(tab), res_table(res)
    {
        size = tab.size() * tab[0].size();
    };

    //return true when it collects every EOS from the workers
    void collect(bool end)
    {
        if (end == true)
            collected++;

        if (collected == size)
        {
            std::swap(table, res_table);
        }
    }
};

}; // namespace FFarm
