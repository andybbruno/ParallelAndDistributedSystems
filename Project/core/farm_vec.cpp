#pragma once
#include <deque>
#include <vector>
#include <iostream>
#include "farm.hpp"
#include "../lib/tools.cpp"

namespace Farm
{
    struct Task
    {
        std::vector<int> *list;
        int begin;
        int end;
        bool swap;

        Task(std::vector<int> *list = nullptr, int a = 0, int b = 0, bool s = false) : list(list), begin(a), end(b) {}

        inline bool operator==(const Task &rhs)
        {
            return (begin == rhs.begin) && (end == rhs.end) && (swap == rhs.swap);
        }
    };

    Task EOS(nullptr, -1, -1, 0);

    enum C2E_Flag
    {
        CONTINUE,
        RESTART,
        EXIT
    };

    class Emitter : IEmitter<Task>
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
            auto task_vec = new std::vector<int>(vec.begin() + a, vec.begin() + b);
            return Task(task_vec, a, b);
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

    class Worker : IWorker<Task, Task>
    {
    public:
        Task compute(Task &tsk)
        {
            bool exchange = false;

            for (int i = tsk.begin; i <= tsk.end; i += 2)
            {
                // tmp.push_back(i);
                if (tsk.list[i] > tsk.list[i + 1])
                {
                    std::swap(tsk.list[i], tsk.list[i + 1]);
                    exchange = true;
                }
            }

            exchange ? (tsk.swap = true) : (tsk.swap = false);
            return tsk;
        }
    };

    class Collector : ICollector<Task, C2E_Flag>
    {
    private:
        std::vector<int> &vec;
        uint nw;
        uint swap = 0;
        uint collected = 0;

    public:
        Collector(std::vector<int> &vec, uint nw) : vec(vec), nw(nw) {}

        C2E_Flag collect(Task const &t)
        {
            collected++;

            if (t.swap)
            {
                swap += t.swap;
                auto list = t.list;
                for (int i = t.begin; i <= t.end; i += 2)
                {
                    vec[i] = (*list)[i];
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