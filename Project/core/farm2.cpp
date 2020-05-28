#pragma once
#include <mutex>
#include <deque>
#include <vector>
#include <iostream>
#include "farm.hpp"

namespace Farm
{
    struct Task
    {
        uint begin;
        uint end;
        bool swap;

        Task(uint a = 0, uint b = 0, bool s = false) : begin(a), end(b) {}

        inline bool operator==(const Task &rhs)
        {
            return (begin == rhs.begin) && (end == rhs.end);
        }
    };

    Task EOS(-1, -1, 0);

    enum C2E_Flag
    {
        CONTINUE,
        RESTART,
        EXIT
    };

    class Emitter : IEmitter<Task>
    {
    private:
        uint curr = 0;
        uint nw = 0;
        std::vector<std::pair<uint, uint>> ranges;
        bool even = true;

    public:
        Emitter(std::vector<int> &vec, uint nw) : nw(nw)
        {
            uint delta = vec.size() / nw;
            uint mod = vec.size() % nw;

            for (int i = 0; i < nw; i++)
            {
                uint a = (i * delta);
                uint b = a + delta - 1;

                ranges.push_back(std::make_pair(a, b));
            }
            if (mod > 0)
            {
                ranges.back().second += mod;
            }
        };

        Task next()
        {
            uint a = ranges[curr].first;
            uint b = ranges[curr].second;

            if (!even)
            {
                a++;
                b++;
                if (curr == (ranges.size() - 1))
                    b--;
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

    class Worker : IWorker<Task, Task>
    {
    private:
        std::vector<int> &vec;

    public:
        Worker(std::vector<int> &vec) : vec(vec) {}
        Task compute(Task &t)
        {
            bool exchange = false;

            for (int i = t.begin; i < t.end; i += 2)
            {
                if (vec[i] > vec[i + 1])
                {
                    std::swap(vec[i], vec[i + 1]);
                    exchange = true;
                }
            }
            // //Odd positions
            // for (int i = begin + 1; i <= end; i += 2)
            // {
            //     if (glob_vec[i] > glob_vec[i + 1])
            //     {
            //         std::swap(glob_vec[i], glob_vec[i + 1]);
            //         exchange = true;
            //     }
            // }

            exchange ? (t.swap = true) : (t.swap = false);
            return t;
        }
    };

    class Collector : ICollector<Task, C2E_Flag>
    {
    private:
        uint nw;
        uint swap = 0;
        uint collected = 0;

    public:
        Collector(uint nw) : nw(nw) {}

        C2E_Flag collect(Task const &t)
        {
            swap += t.swap;
            collected++;
            // std::cout << "swap " << swap << std::endl;
            // std::cout << "size " << dq.size() << std::endl;
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