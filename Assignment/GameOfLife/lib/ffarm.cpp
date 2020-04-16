#pragma once
#include <mutex>
#include "ffarm.hpp"
#include "tools.hpp"

std::mutex tab_m;
std::mutex res_tab_m;

// Farm with Feedback
namespace FFarm
{
class Emitter : IEmitter<Chunk>
{
private:
    uint8_t curr = 1;
    uint8_t size = 0;
    Table &table;

public:
    Emitter(Table &t) : table(t)
    {
        size = table.size() - 2;
    };

    Chunk next()
    {
        Chunk ck{curr, table[curr - 1], table[curr], table[curr + 1]};
        curr++;
        return ck;
    }

    bool hasNext()
    {
        return curr <= size;
    }

    void restart()
    {
        curr = 1;
    }
};

class Worker : IWorker<Chunk, RowID>
{
public:
    RowID compute(Chunk const &c)
    {
        auto curr_idx = c.id_curr;
        auto prev = c.prev;
        auto curr = c.curr;
        auto next = c.next;

        auto size = curr.size();
        Row partialSum(size, 0);
        Row neigh(size, 0);
        Row res(size, 0);

        for (size_t i = 1; i < size - 1; i++)
        {
            partialSum[i] = curr[i] + prev[i] + next[i];
        }

        for (size_t i = 1; i < size - 1; i++)
        {
            neigh[i] = partialSum[i - 1] + partialSum[i] + partialSum[i + 1];
        }

        for (size_t i = 1; i < size - 1; i++)
        {
            // if (curr[i] == ON)
            // {
            //     neigh[i] -= 1;
            // }
            neigh[i] -= curr[i];
        }

        for (size_t i = 1; i < size - 1; i++)
        {
            // if ((curr[i] == ON) & ((neigh[i] < 2) | (neigh[i] > 3)))
            // {
            //     res[i] = OFF;
            // }
            // else if ((curr[i] == OFF) & (neigh[i] == 3))
            // {
            //     res[i] = ON;
            // }
            auto neig = neigh[i];
            auto alive = curr[i];

            if (alive == ON)
            {
                if ((neig < 2) | (neig > 3))
                {
                    res[i] = OFF;
                }
                else if ((neig == 2) | (neig == 3))
                {
                    res[i] = ON;
                }
            }
            else
            {
                if (neig == 3)
                {
                    res[i] = ON;
                }
            }
        }
        return RowID{curr_idx, res};
    }
};

class Collector : ICollector<RowID, bool>
{
private:
    uint8_t size = 0;
    uint8_t collected = 0;
    Table &table;
    Table &res_table;

public:
    Collector(Table &tab, Table &res) : table(tab), res_table(res)
    {
        size = tab.size() - 2;
    };

    // return true when it finishes
    bool collect(RowID const &r)
    {
        auto pos = r.first;
        auto row = r.second;

        res_tab_m.lock();
        res_table[pos] = row;
        res_tab_m.unlock();
        collected++;

        if (collected == size)
        {
            tab_m.lock();
            res_tab_m.lock();
            std::swap(table, res_table);
            // table = res_table;
            tab_m.unlock();
            res_tab_m.unlock();
            collected = 0;
            return true;
        }
        return false;
    }
};
}; // namespace FFarm
