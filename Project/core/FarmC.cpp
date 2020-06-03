#pragma once
#include <deque>
#include <vector>
#include <iostream>
#include "IFarm.hpp"
#include "../lib/tools.cpp"

namespace Farm
{
    struct Chunk
    {
        std::vector<int> *list;
        int begin;
        int end;
        bool swap;

        Chunk(std::vector<int> *list = nullptr, int a = 0, int b = 0, bool s = false) : list(list), begin(a), end(b) {}

        inline bool operator==(const Chunk &rhs)
        {
            return (begin == rhs.begin) && (end == rhs.end) && (swap == rhs.swap);
        }
    };

    Chunk EOS(nullptr, -1, -1, 0);

    enum C2E_Flag
    {
        CONTINUE,
        RESTART,
        EXIT
    };

    class Emitter : IEmitter<Chunk>
    {
    private:
        std::vector<int> &vec;
        uint curr = 0;
        uint nw = 0;
        std::vector<std::pair<uint, uint>> ranges_odd;
        std::vector<std::pair<uint, uint>> ranges_even;
        bool even = true;
        uint print = 0;

    public:
        Emitter(std::vector<int> &vec, uint nw) : vec(vec), nw(nw)
        {
            auto n = vec.size();
            ranges_even = tools::make_ranges(n - 1, nw, 2, 0);
            ranges_odd = tools::make_ranges(n - 1, nw, 2, 1);
        };

        Chunk next()
        {
            int a;
            int b;

            if (even)
            {
                a = ranges_even[curr].first;
                b = ranges_even[curr].second;
            }
            else
            {
                a = ranges_odd[curr].first;
                b = ranges_odd[curr].second;
            }

            curr++;
            auto begin = vec.begin() + a;
            auto end = vec.begin() + b + 2;
            

            auto task_vec = new std::vector<int>(begin, end);
            return Chunk(task_vec, a, b);
        }

        bool hasNext()
        {
            return curr < nw;
        }

        void restart()
        {
            curr = 0;
            even = !even;
        }
    };

    class Worker : IWorker<Chunk, Chunk>
    {
    public:
        Chunk compute(Chunk &tsk)
        {
            bool exchange = false;

            std::vector<int> &list = *tsk.list;
            for (int i = 0; i < list.size(); i += 2)
            {
                if (list[i] > list[i + 1])
                {
                    std::swap(list[i], list[i + 1]);
                    exchange = true;
                }
            }

            exchange ? (tsk.swap = true) : (tsk.swap = false);
            return tsk;
        }
    };

    class Collector : ICollector<Chunk, C2E_Flag>
    {
    private:
        std::vector<int> &vec;
        uint nw;
        uint swap = 0;
        uint collected = 0;

    public:
        Collector(std::vector<int> &vec, uint nw) : vec(vec), nw(nw) {}

        C2E_Flag collect(Chunk const &t)
        {
            collected++;

            if (t.swap)
            {
                swap++;
                std::vector<int> &list = *t.list;
                int k = 0;
                for (int i = t.begin; i < t.begin + list.size(); i++)
                {
                    vec[i] = list[k];
                    k++;
                }
            }
            if ((swap > 0) && (collected == nw))
            {
                swap = 0;
                collected = 0;
                return RESTART;
            }
            else if ((swap == 0) && (collected == nw))
            {
                swap = 0;
                collected = 0;
                return EXIT;
            }
            else
            {
                return CONTINUE;
            }
        }
    };
}; // namespace Farm