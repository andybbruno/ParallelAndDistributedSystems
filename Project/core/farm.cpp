#pragma once
#include <mutex>
#include <deque>
#include "farm.hpp"
#include "../lib/tools.cpp"

namespace Farm
{
    struct Task
    {
        std::vector<int> chunk;
        uint begin;
        uint end;
        bool swap;
        bool EOS;

        Task(std::vector<int> const &v, uint begin, uint end, bool eos = false) : begin(begin), end(end), EOS(eos)
        {
            auto it = v.begin();
            chunk = std::vector<int>(it + begin, it + end + 1);
        };

        Task() {}

        inline bool operator==(const Task &rhs)
        {
            return (chunk.size() == rhs.chunk.size()) && (EOS = rhs.EOS);
        }
    };

    Task EOS(std::vector<int>(1, 1), 1, 1, true);

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
        std::vector<Task> even_list;
        std::vector<Task> odd_list;
        std::vector<std::pair<uint, uint>> ranges;
        bool even = true;

    public:
        Emitter(std::vector<int> &vec, uint nw) : vec(vec), nw(nw)
        {
            ranges = tools::make_ranges(vec.size(), nw);
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
            return Task(vec, a, b);
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
        Task compute(Task &t)
        {
            bool exchange = false;

            for (int i = 0; i < t.chunk.size() - 1; i += 2)
            {
                if (t.chunk[i] > t.chunk[i + 1])
                {
                    std::swap(t.chunk[i], t.chunk[i + 1]);
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
        std::vector<int> &vec;
        std::vector<int> res;
        std::deque<Task> dq;

        void recombine()
        {
            res = vec;
            for (int j = 0; j <= dq.size(); j++)
            {
                auto x = dq.front();
                dq.pop_front();
                for (int i = x.begin; i <= x.end; i++)
                {
                    res[i] = x.chunk[i - x.begin];
                }
            }
            std::swap(vec, res);
        }

    public:
        Collector(std::vector<int> &vec, uint nw) : vec(vec), nw(nw) {}

        C2E_Flag collect(Task const &t)
        {
            dq.push_back(t);
            swap += t.swap;
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
                swap = 0;
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