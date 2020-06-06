/**
 * @author Andrea Bruno
 * @brief MASTER-WORKERS + TASKS [IMPLEMENTATION]
 * @date 06-2020
 */

#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include <random>
#include <algorithm>
#include "core/MaWo.cpp"
#include "lib/buffer.cpp"
#include "lib/tools.cpp"
#include "lib/utimer.cpp"

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

    // Create Master and Workers
    MaWo::Master master(vec.size(), nw);
    MaWo::Worker worker(vec);

    std::vector<Buffer<MaWo::Task>> m2w_buff(nw); // master to worker buffer
    std::vector<Buffer<bool>> w2m_buff(nw);       // master to worker buffer

    /**
     * @brief Lambda of the Master
     * 
     */
    auto mst = [&](MaWo::Master m) {
        bool swap = true;

        // while a swap occurred
        while (swap)
        {
            /**
             * @brief SEND DATA EVEN PHASE
             * 
             */
            swap = false;
            int turn = 0;

            // while the Master has something to emit
            while (m.hasNext())
            {
                // get this element
                auto job = m.next();

                // send it to the worker buffer in position #turn
                m2w_buff[turn].send(job);

                //update the #turn
                turn = (++turn) % nw;
            }
            turn = 0;
            uint received = 0;

            /**
             * @brief RECEIVE DATA EVEN PHASE
             * 
             */
            while (true)
            {
                // if the buffer in position #turn is empty then skip the current computation
                if (w2m_buff[turn].empty())
                {
                    turn = (++turn) % nw;
                    continue;
                }
                // receive #res from the buffer in position #turn
                bool res = w2m_buff[turn].receive();

                // do the logic-OR
                swap |= res;

                // if the Master has received #nw booleans then exit
                if (++received == nw)
                    break;
            }

            // Change phase and repeat for the odd indexes
            m.restart();

            /**
             * @brief SEND DATA ODD PHASE
             * 
             */
            while (m.hasNext())
            {
                auto job = m.next();
                m2w_buff[turn].send(job);
                turn = (++turn) % nw;
            }

            received = 0;

            /**
             * @brief RECEIVE DATA ODD PHASE
             * 
             */
            while (true)
            {
                if (w2m_buff[turn].empty())
                {
                    turn = (++turn) % nw;
                    continue;
                }
                bool res = w2m_buff[turn].receive();
                swap |= res;

                if (++received == nw)
                    break;
            }

            m.restart();
        }

        // send EOS to the Workers
        for (int i = 0; i < nw; i++)
        {
            m2w_buff[i].send(MaWo::EOS);
        }
    };

    /**
     * @brief Lamdba of the Worker
     * @param wid the ID of the Worker
     */
    auto work = [&](MaWo::Worker w, int wid) {
        while (true)
        {
            // receive something from the Master
            auto job = m2w_buff[wid].receive();

            // if #job equals EOS interrupt the Worker
            if (job == MaWo::EOS)
            {
                break;
            }

            // compute the assigned #job
            bool res = w.compute(job);

            // send to the Master
            w2m_buff[wid].send(res);
        }
    };

    utimer u(std::to_string(nw) + "," + std::to_string(dim));

    // create one thread for the Master
    std::thread mst_thr(mst, master);

    // create nw thread for the Workers
    std::vector<std::thread> tids;
    for (int i = 0; i < nw; i++)
    {
        tids.push_back(std::thread(work, worker, i));
    }

    // join all threads
    mst_thr.join();

    for (int i = 0; i < nw; i++)
    {
        tids[i].join();
    }

    // !! NOTE !!
    //
    // To check that the array is sorted, please remove the comment to the following line.
    //

    // assert(std::is_sorted(vec.begin(), vec.end()));
    return 0;
}