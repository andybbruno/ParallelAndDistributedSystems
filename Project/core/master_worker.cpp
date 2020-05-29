#pragma once
#include <deque>
#include <vector>
#include <iostream>
#include "master_worker.hpp"
#include "../lib/tools.cpp"

namespace MaWo
{
    struct Task
    {
        int begin;
        int end;
        bool swap;

        Task(int a = 0, int b = 0, bool s = false) : begin(a), end(b) {}

        inline bool operator==(const Task &rhs)
        {
            return (begin == rhs.begin) && (end == rhs.end);
        }
    };

    Task EOS(-1, -1, 0);

    class Master : IMaster<Task>
    {
    private:
        uint curr = 0;
        uint nw = 0;
        std::vector<std::pair<uint, uint>> ranges_odd;
        std::vector<std::pair<uint, uint>> ranges_even;
        bool even = true;
        uint print = 0;
        bool &swap;

    public:
        Master(std::vector<int> &vec, bool &swap, uint nw) : swap(swap), nw(nw)
        {
            auto n = vec.size();
            ranges_even = tools::make_ranges(n - 1, nw, 2, 0);
            ranges_odd = tools::make_ranges(n - 1, nw, 2, 1);
        };

        Task next()
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
            return Task(a, b);
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

    class Worker : IWorker<Task>
    {
    private:
        std::vector<int> &vec;
        bool &swap;

    public:
        Worker(std::vector<int> &vec, bool &swap) : vec(vec), swap(swap) {}
        void compute(Task &t)
        {
            bool exchange = false;

            std::vector<int> tmp;
            for (int i = t.begin; i <= t.end; i += 2)
            {
                // tmp.push_back(i);
                if (vec[i] > vec[i + 1])
                {
                    std::swap(vec[i], vec[i + 1]);
                    exchange = true;
                }
            }
            swap = exchange;
        }
    };
}; // namespace MaWo