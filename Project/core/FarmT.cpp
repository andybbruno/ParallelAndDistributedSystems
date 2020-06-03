#pragma once
#include <deque>
#include <vector>
#include <iostream>
#include "IFarm.hpp"
#include "../lib/tools.cpp"

namespace Farm
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
        std::vector<std::pair<uint, uint>> ranges_odd;
        std::vector<std::pair<uint, uint>> ranges_even;
        bool even = true;
        uint print = 0;

    public:
        Emitter(std::vector<int> &vec, uint nw) : nw(nw)
        {
            // uint delta = vec.size() / nw;
            // uint mod = vec.size() % nw;

            // for (int i = 0; i < nw; i++)
            // {
            //     uint a = (i * delta);
            //     uint b = a + delta - 1;

            //     ranges.push_back(std::make_pair(a, b));
            // }
            // if (mod > 0)
            // {
            //     ranges.back().second += mod;
            // }

            auto n = vec.size();
            ranges_even = tools::make_ranges(n - 1, nw, 2, 0);
            ranges_odd = tools::make_ranges(n - 1, nw, 2, 1);

            // for (auto x : ranges_even)
            // {
            //     std::cout << x.first << " - " << x.second << "\n";
            // }
            // std::cout << "*******" << std::endl;
            // for (auto x : ranges_odd)
            // {
            //     std::cout << x.first << " - " << x.second << "\n";
            // }
            // std::cout << std::endl;
            // std::cout << std::endl;
            // std::cout << std::endl;
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

            // if (!even)
            // {
            //     a++;
            //     b++;
            //     if (curr == (ranges.size() - 1))
            //         b--;
            // }

            // if (print <= (2 * nw) - 1)
            // {
            //     std::cout << a << " - " << b << std::endl;
            //     --print;
            // }

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
            // mtx.lock();
            // for (auto z : tmp)
            // {
            //     std::cout << z << " - ";
            // }
            // std::cout << std::endl;
            // mtx.unlock();

            // for (int i = t.begin + 1; i < t.end + 1; i += 2)
            // {
            //     if (vec[i] > vec[i + 1])
            //     {
            //         std::swap(vec[i], vec[i + 1]);
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