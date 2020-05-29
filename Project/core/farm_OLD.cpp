#pragma once
#include <mutex>
#include <deque>
#include <vector>
#include <iostream>
#include <climits>
#include "farm.hpp"

namespace Farm
{
    struct Task
    {
        std::deque<uint> swaps;
        uint begin;
        uint end;

        Task() : begin(0), end(0) {}

        Task(uint a, uint b) : begin(a), end(b) {}

        inline bool operator==(const Task &rhs)
        {
            return (begin == rhs.begin) && (end == rhs.end);
        }
    };

    Task EOS(UINT_MAX, UINT_MAX);

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
        std::vector<std::pair<uint, uint>> ranges;
        bool even = true;

    public:
        Emitter(std::vector<int> &vec, uint nw) : vec(vec), nw(nw)
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
            int a = ranges[curr].first;
            int b = ranges[curr].second;

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
        std::vector<int> const &v;

    public:
        Worker(std::vector<int> const &v) : v(v) {}

        Task compute(Task &t)
        {
            bool exchange = false;

            for (int i = t.begin; i < t.end; i += 2)
            {
                if (v[i] > v[i + 1])
                {
                    t.swaps.push_back(i);
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

            return t;
        }
    };

    class Collector : ICollector<Task, C2E_Flag>
    {
    private:
        uint nw;
        uint swap = 0;
        std::vector<int> &vec;
        std::deque<Task> dq;

        inline void recombine()
        {
            std::vector<int> res(vec);

            for (int i = 0; i <= dq.size(); i++)
            {
                auto x = dq.front();
                dq.pop_front();
                for (auto sw : x.swaps)
                {
                    std::swap(res[sw], res[sw + 1]);
                }
            }
            vec = res;
        }

    public:
        Collector(std::vector<int> &vec, uint nw) : vec(vec), nw(nw) {}

        C2E_Flag collect(Task const &t)
        {
            dq.push_back(t);
            swap += t.swaps.size();
            // std::cout << "swap " << swap << std::endl;
            // std::cout << "size " << dq.size() << std::endl;
            if ((swap > 0) && (dq.size() == nw))
            {
                swap = 0;
                recombine();
                return RESTART;
            }
            else if ((swap == 0) && (dq.size() == nw))
            {
                recombine();
                return EXIT;
            }
            else
            {
                return CONTINUE;
            }
        }
    };
}; // namespace Farm