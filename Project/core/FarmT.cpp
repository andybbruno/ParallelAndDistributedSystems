/**
 * @author Andrea Bruno
 * @brief FARM WITH FEEDBACK + TASKS
 * @date 06-2020
 */

#pragma once
#include <deque>
#include <vector>
#include <iostream>
#include "IFarm.hpp"
#include "../lib/tools.cpp"

namespace Farm
{
    /**
     * @brief Task data structure 
     * 
     */
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

    /**
     * @brief End Of Stream. Used to interrupt the computation.
     * 
     */
    Task EOS(-1, -1, 0);

    /**
     * @brief Collector To Emitter communication flags.
     * 
     */
    enum C2E_Flag
    {
        CONTINUE,
        RESTART,
        EXIT
    };

    /**
     * @brief The Emitter entity of the Farm.
     * 
     */
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
        /**
         * @brief Construct a new Emitter object. 
         * 
         * @param vec the global array
         * @param nw  number of workers
         */
        Emitter(std::vector<int> &vec, uint nw) : nw(nw)
        {
            auto n = vec.size();
            ranges_even = tools::make_ranges(n - 1, nw, 2, 0);
            ranges_odd = tools::make_ranges(n - 1, nw, 2, 1);
        };

        /**
         * @brief Creates a new Task based on the current phase (Odd/Even). 
         * 
         * @return a new Task
         */
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

        /**
         * @brief Check if there is something else to produce.
         * 
         */
        bool hasNext()
        {
            return curr < nw;
        }

        /**
         * @brief This function restarts the Emitter. This means that the Emitter will change phase (Odd/Even).
         * 
         */
        void restart()
        {
            curr = 0;
            even = !even;
        }
    };


    /**
     * @brief The Worker entity of the Farm.
     * 
     */
    class Worker : IWorker<Task, Task>
    {
    private:
        std::vector<int> &vec;

    public:
        /**
         * @brief Construct a new Worker object.
         * 
         * @param vec the global array.
         */
        Worker(std::vector<int> &vec) : vec(vec) {}

        /**
         * @brief Scan every element in the list inside the Task. In case two adjacents elements are out of order, it will swap them.
         * 
         * @param t input Task
         * @return Task output Task
         */
        Task compute(Task &t)
        {
            bool exchange = false;

            std::vector<int> tmp;
            for (int i = t.begin; i <= t.end; i += 2)
            {
                if (vec[i] > vec[i + 1])
                {
                    std::swap(vec[i], vec[i + 1]);
                    exchange = true;
                }
            }

            exchange ? (t.swap = true) : (t.swap = false);
            return t;
        }
    };

    /**
     * @brief The Collector entity of the Farm.
     * 
     */
    class Collector : ICollector<Task, C2E_Flag>
    {
    private:
        uint nw;
        uint swap = 0;
        uint collected = 0;

    public:
        /**
         * @brief Construct a new Collector object
         * 
         * @param nw number of workers
         */
        Collector(uint nw) : nw(nw) {}

        /**
         * @brief This function collects the Tasks from the Workers and in case either restarts the Emitter or asks to terminate the computation.
         * 
         * @param t input Task
         * @return C2E_Flag communication flags
         */
        C2E_Flag collect(Task const &t)
        {
            swap += t.swap;
            collected++;

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