#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <cstdlib>
#include <ff/utils.hpp>
#include <ff/parallel_for.hpp>
#include "lib/tools.cpp"
#include "lib/utimer.cpp"

using namespace ff;

struct Task
{
    int begin;
    int end;
    bool &swap;

    Task(int a, int b, bool &swap) : begin(a), end(b), swap(swap) {}

    inline bool operator==(const Task &rhs)
    {
        return (begin == rhs.begin) && (end == rhs.end);
    }
};

struct Emitter : ff_node_t<bool, Task>
{
    uint curr = 0;
    uint nw = 0;
    uint received = 0;
    std::vector<std::pair<uint, uint>> ranges_odd;
    std::vector<std::pair<uint, uint>> ranges_even;
    bool even = true;
    bool swap = true;

    Master(uint n, uint nw) : nw(nw)
    {
        ranges_even = tools::make_ranges(n - 1, nw, 2, 0);
        ranges_odd = tools::make_ranges(n - 1, nw, 2, 1);
        received = nw;
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

        return new Task(a, b, swap);
    }

    inline void restart()
    {
        curr = 0;
        even = !even;
        received = 0;
        swap = false;
    }

    Task *svc(bool *exchange)
    {
        // std::cout << "MASTER" << std::endl;
        if (exchange != nullptr)
        {
            swap |= *exchange;
            if (++received == nw)
            {
                if ((swap == false) && (even == false))
                {
                    return EOS;
                }
                else
                {
                    for (int i = 0; i < nw; i++)
                    {
                        ff_send_out(generateTask());
                    }
                    restart();
                }
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
    bool exchange;

    Worker(std::vector<int> &vec) : vec(vec) {}
    bool *svc(Task *t)
    {
        // std::cout << "WORKER" << std::endl;
        bool exchange = false;
        for (int i = t->begin; i <= t->end; i += 2)
        {
            if (vec[i] > vec[i + 1])
            {
                std::swap(vec[i], vec[i + 1]);
                exchange = true;
            }
        }
        return new bool(exchange);
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
    // tools::print(vec);
    Master master(dim, nw);
    std::vector<std::unique_ptr<ff_node>> W;
    for (size_t i = 0; i < nw; ++i)
    {
        W.push_back(make_unique<Worker>(vec));
    }

    ff_Farm<Task> farm(std::move(W), master);
    farm.remove_collector();
    farm.wrap_around();

    utimer u(std::to_string(nw) + "," + std::to_string(dim));
    if (farm.run_and_wait_end() < 0)
    {
        error("running farm");
        return -1;
    }
    
    // assert(std::is_sorted(vec.begin(), vec.end()));
    return 0;
}