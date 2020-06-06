/**
 * @author Andrea Bruno
 * @brief MASTER-WORKER using TASKS
 * @date 06-2020
 */

#pragma once
#include <deque>
#include <vector>
#include <iostream>
#include "IMaWo.hpp"
#include "../lib/tools.cpp"

namespace MaWo
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
     * @brief The Master entity
     * 
     */
    class Master : IMaster<Task>
    {
    private:
        uint curr = 0;
        uint nw = 0;
        std::vector<std::pair<uint, uint>> ranges_odd;
        std::vector<std::pair<uint, uint>> ranges_even;
        bool even = true;

    public:
        /**
         * @brief Construct a new Master object
         * 
         * @param n number of elements in the array
         * @param nw numer of workers
         */

        Master(uint n, uint nw) : nw(nw)
        {
            ranges_even = tools::make_ranges(n - 1, nw, 2, 0);
            ranges_odd = tools::make_ranges(n - 1, nw, 2, 1);
        };

        /**
         * @brief Creates a new Task based on the current phase (Odd/Even). 
         * 
         * @return Task 
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
         * @brief This function restarts the Master. This means that the Master will change phase (Odd/Even).
         * 
         */
        void restart()
        {
            curr = 0;
            even = !even;
        }
    };


    /**
     * @brief The Worker entity
     * 
     */
    class Worker : IWorker<Task, bool>
    {
    private:
        std::vector<int> &vec;

    public:
        /**
         * @brief Construct a new Worker object.
         * 
         * @param vec the global array
         */
        Worker(std::vector<int> &vec) : vec(vec) {}

        /**
         * @brief Scan every element in the list inside the Task. In case two adjacents elements are out of order, it will swap them.
         * 
         * @param t input Task
         * @return true if swap occured
         */
        bool compute(Task &t)
        {
            bool exchange = false;
            for (int i = t.begin; i <= t.end; i += 2)
            {
                if (vec[i] > vec[i + 1])
                {
                    std::swap(vec[i], vec[i + 1]);
                    exchange = true;
                }
            }
            return exchange;
        }
    };

}; // namespace MaWo