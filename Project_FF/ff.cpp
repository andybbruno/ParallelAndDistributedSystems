#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <cstdlib>
#include <ff/utils.hpp>
#include <ff/parallel_for.hpp>
#include "lib/ff_tools.cpp"

using namespace ff;
using ull = unsigned long long;
using Range = std::pair<ull, ull>;

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

struct Master : ff_node_t<Task>
{
    uint curr = 0;
    uint nw = 0;
    uint received = 0;
    std::vector<std::pair<uint, uint>> ranges_odd;
    std::vector<std::pair<uint, uint>> ranges_even;
    bool even = true;
    bool swap = true;

    Task *generateTask()
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

    void restart()
    {
        curr = 0;
        even = !even;
    }

    Master(uint n, uint nw) : nw(nw)
    {
        ranges_even = tools::make_ranges(n - 1, nw, 2, 0);
        ranges_odd = tools::make_ranges(n - 1, nw, 2, 1);
    };

    Task *svc(bool *exchange)
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
        return GO_ON;
    }
};

struct Worker : ff_node_t<Task>
{
    Task *svc(Task *task)
    {
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

    return 0;
}