/**
 * @author Andrea Bruno
 * @brief FARM WITH FEEDBACK + CHUNKS
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
     * @brief Chunk data structure
     * 
     */
    struct Chunk
    {
        std::vector<int> *list;
        int begin;
        int end;
        bool swap;

        Chunk(std::vector<int> *list = nullptr, int a = 0, int b = 0, bool s = false) : list(list), begin(a), end(b) {}

        inline bool operator==(const Chunk &rhs)
        {
            return (begin == rhs.begin) && (end == rhs.end) && (swap == rhs.swap);
        }
    };

    /**
     * @brief End Of Stream. Used to interrupt the computation.
     * 
     */
    Chunk EOS(nullptr, -1, -1, 0);

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
    class Emitter : IEmitter<Chunk>
    {
    private:
        std::vector<int> &vec;
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
        Emitter(std::vector<int> &vec, uint nw) : vec(vec), nw(nw)
        {
            auto n = vec.size();
            ranges_even = tools::make_ranges(n - 1, nw, 2, 0);
            ranges_odd = tools::make_ranges(n - 1, nw, 2, 1);
        };

        /**
         * @brief Creates a new Chunk based on the current phase (Odd/Even). 
         * Note that the vector within the Chunk is copied by the original array.
         * 
         * @return a new Chunk
         */
        Chunk next()
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
            auto begin = vec.begin() + a;
            auto end = vec.begin() + b + 2;

            auto task_vec = new std::vector<int>(begin, end);
            return Chunk(task_vec, a, b);
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
    class Worker : IWorker<Chunk, Chunk>
    {
    public:
        /**
         * @brief Scan every element in the list inside the Chunk. In case two adjacents elements are out of order, it will swap them.
         * 
         * @param cnk input Chunk
         * @return Chunk output Chunk
         */
        Chunk compute(Chunk &cnk)
        {
            bool exchange = false;

            std::vector<int> &list = *cnk.list;
            for (int i = 0; i < list.size(); i += 2)
            {
                if (list[i] > list[i + 1])
                {
                    std::swap(list[i], list[i + 1]);
                    exchange = true;
                }
            }

            exchange ? (cnk.swap = true) : (cnk.swap = false);
            return cnk;
        }
    };

    /**
     * @brief The Collector entity of the Farm.
     * 
     */
    class Collector : ICollector<Chunk, C2E_Flag>
    {
    private:
        std::vector<int> &vec;
        uint nw;
        uint swap = 0;
        uint collected = 0;

    public:
        /**
         * @brief Construct a new Collector object
         * 
         * @param vec the global array
         * @param nw number of workers
         */
        Collector(std::vector<int> &vec, uint nw) : vec(vec), nw(nw) {}

        /**
         * @brief This function collects the Chunks from the Workers and in case either restarts the Emitter or asks to terminate the computation.
         * 
         * @param cnk input Chunk
         * @return C2E_Flag communication flags
         */
        C2E_Flag collect(Chunk const &cnk)
        {
            collected++;

            if (cnk.swap)
            {
                swap++;
                std::vector<int> &list = *cnk.list;
                int k = 0;
                for (int i = cnk.begin; i < cnk.begin + list.size(); i++)
                {
                    vec[i] = list[k];
                    k++;
                }
            }
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