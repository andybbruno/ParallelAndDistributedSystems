/**
 * @author Andrea Bruno
 * @brief FASTFLOW FARM WITH FEEDBACK + TASKS [IMPLEMENTATION]
 * @date 06-2020
 */

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

/**
 * @brief Task data structure
 * 
 */
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

/**
 * @brief Communication flags.
 * 
 */
enum Flag
{
    EXIT,
    RESTART
};

/**
 * @brief Emitter entity
 * 
 */
struct Emitter : ff_node_t<Flag, Task>
{
    uint curr = 0;
    uint nw = 0;
    std::vector<std::pair<uint, uint>> ranges_odd;
    std::vector<std::pair<uint, uint>> ranges_even;
    bool even = true;

    /**
     * @brief Construct a new Emitter object.
     * 
     * @param n number of elements in the array
     * @param nw number of workers
     */
    Emitter(uint n, uint nw) : nw(nw)
    {
        ranges_even = tools::make_ranges(n - 1, nw, 2, 0);
        ranges_odd = tools::make_ranges(n - 1, nw, 2, 1);
    };

    /**
     * @brief Creates a new Task based on the current phase (Odd/Even). 
     * 
     * @return Task* a pointer to a new Task
     */
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

    /**
     * @brief This function restarts the Emitter. This means that the Emitter will change phase (Odd/Even).
     * 
     */
    inline void restart()
    {
        curr = 0;
        even = !even;
    }

    /**
     * @brief FastFlow service function: contains the Emitter logic.
     * 
     * The Emitter creates new Tasks based on the actual phase.
     * 
     * @param flag the input flag
     * @return Task* a pointer to a Task
     */
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

/**
 * @brief Worker entitiy.
 * 
 */
struct Worker : ff_node_t<Task, bool>
{
    std::vector<int> &vec;

    /**
     * @brief Construct a new Worker object.
     * 
     * @param vec the global array
     */
    Worker(std::vector<int> &vec) : vec(vec) {}

    /**
     * @brief FastFlow service function: contains the Worker logic.
     * 
     * Scan every element in the list inside the Task. In case two adjacents elements are out of order, it will swap them.
     * 
     * @param t the input Task
     * @return true if a swap occurred
     */
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

/**
 * @brief Collector entity
 * 
 */
struct Collector : ff_node_t<bool, Flag>
{
    bool swap = true;
    bool even = true;
    int nw;
    uint received = 0;

    /**
     * @brief Construct a new Collector object
     * 
     * @param nw number of workers
     */
    Collector(int nw) : nw(nw) {}

    /**
     * @brief FastFlow service function: contains the Collector logic.
     * 
     * This function collects the Tasks from the Workers and in case either restarts the Emitter or asks to terminate the computation.
     * 
     * @param t the input Task
     * @return Flag* a pointer to a Flag used to communicate with the Emitter
     */
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
                    return new Flag(RESTART);
                }
            }
        }
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

    // Create a random vector
    std::vector<int> vec = tools::rand_vec(dim, range);

    // Create Emitter, Workers and Collector
    Emitter emitter(dim, nw);
    Collector collector(nw);
    std::vector<std::unique_ptr<ff_node>> W;
    for (size_t i = 0; i < nw; ++i)
    {
        W.push_back(make_unique<Worker>(vec));
    }

    // Create the Farm 
    ff_Farm<Task> farm(std::move(W), emitter, collector);

    // Add a feedback channel
    farm.wrap_around();

    utimer u(std::to_string(nw) + "," + std::to_string(dim));
    if (farm.run_and_wait_end() < 0)
    {
        error("running farm");
        return -1;
    }

    // !! NOTE !!
    //
    // To check that the array is sorted, please remove the comment to the following line.
    //

    // assert(std::is_sorted(vec.begin(), vec.end()));
    return 0;
}