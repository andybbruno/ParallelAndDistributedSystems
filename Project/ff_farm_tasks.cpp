#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <cstdlib>
#include <ff/utils.hpp>
#include <ff/ff.hpp>
#include "lib/tools.cpp"
#include "lib/utimer.cpp"

using namespace ff;

struct Task
{
    int begin;
    int end;
    bool swap;
    Task(int a, int b, bool swap) : begin(a), end(b), swap(swap) {}

    inline bool operator==(const Task &rhs)
    {
        return (begin == rhs.begin) && (end == rhs.end);
    }
};

enum Flag
{
    EXIT,
    RESTART
};

struct Emitter : ff_node_t<Flag, Task>
{
    uint curr = 0;
    uint nw = 0;
    std::vector<std::pair<uint, uint>> ranges_odd;
    std::vector<std::pair<uint, uint>> ranges_even;
    bool even = true;

    Emitter(uint n, uint nw) : nw(nw)
    {
        ranges_even = tools::make_ranges(n - 1, nw, 2, 0);
        ranges_odd = tools::make_ranges(n - 1, nw, 2, 1);
    };

    inline Task *generateTask()
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

        return new Task(a, b, false);
    }

    inline void restart()
    {
        curr = 0;
        even = !even;
    }

    Task *svc(Flag *flag)
    {
        if (flag != nullptr)
        {
            if (*flag == RESTART)
            {
                for (int i = 0; i < nw; i++)
                {
                    ff_send_out(generateTask());
                }
                restart();
            }
            else if (*flag == EXIT)
            {
                return EOS;
            }
        }
        else
        {
            for (int i = 0; i < nw; i++)
            {
                ff_send_out(generateTask());
            }
            restart();
        }
        return GO_ON;
    }
};

struct Worker : ff_node_t<Task, bool>
{
    std::vector<int> &vec;

    Worker(std::vector<int> &vec) : vec(vec) {}
    bool *svc(Task *t)
    {
        bool exc = false;
        for (int i = t->begin; i <= t->end; i += 2)
        {
            if (vec[i] > vec[i + 1])
            {
                std::swap(vec[i], vec[i + 1]);
                exc = true;
            }
        }
        return new bool(exc);
    }
};

struct Collector : ff_node_t<bool, Flag>
{
    bool swap = true;
    bool even = true;
    int nw;
    uint received = 0;

    Collector(int nw) : nw(nw) {}

    Flag *svc(bool *t)
    {
        if (t != nullptr)
        {
            swap |= *t;

            if (++received == nw)
            {
                if (swap == false)
                {
                    return new Flag(EXIT);
                }
                else
                {
                    received = 0;
                    swap = false;
                    even = !even;
                    // delete t;
                    return new Flag(RESTART);
                }
            }
        }
        // delete t;
        return GO_ON;
    }
};

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cout << "\nUsage:   #Workers   #Elements   Range   Seed (0 means random)\n\n";
        exit(EXIT_FAILURE);
    }

    int nw = atoi(argv[1]);
    int dim = atoi(argv[2]);
    int range = atoi(argv[3]);
    if (range == 0)
        range = INT16_MAX;
    int seed = atoi(argv[4]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);

    if ((nw >= dim))
    {
        std::cout << "\n Too many workers! \n\n";
        exit(EXIT_FAILURE);
    }
    if (range < 0)
    {
        std::cout << "\n Range must be either positive or '0' for random! \n\n";
        exit(EXIT_FAILURE);
    }

    std::vector<int> vec = tools::rand_vec(dim, range);
    Emitter emitter(dim, nw);
    Collector collector(nw);
    std::vector<std::unique_ptr<ff_node>> W;
    for (size_t i = 0; i < nw; ++i)
    {
        W.push_back(make_unique<Worker>(vec));
    }

    ff_Farm<Task> farm(std::move(W), emitter, collector);
    farm.wrap_around();

    utimer u(std::to_string(nw) + "," + std::to_string(dim));
    if (farm.run_and_wait_end() < 0)
    {
        error("running farm");
        return -1;
    }

    assert(std::is_sorted(vec.begin(), vec.end()));
    return 0;
}