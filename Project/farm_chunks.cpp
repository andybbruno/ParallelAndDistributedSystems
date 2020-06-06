/**
 * @author Andrea Bruno
 * @brief FARM WITH FEEDBACK + CHUNKS [IMPLEMENTATION]
 * @date 06-2020
 */

#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include <algorithm>
#include "core/FarmC.cpp"
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

    // Create Emitter, Worker and Collector
    Farm::Emitter emitter(vec, nw);
    Farm::Worker worker;
    Farm::Collector collector(vec, nw);

    std::vector<Buffer<Farm::Chunk>> e2w_buff(nw); // emitter to worker buffer
    std::vector<Buffer<Farm::Chunk>> w2c_buff(nw); // worker to collector buffer
    Buffer<Farm::C2E_Flag> c2e_buff;               // collector to emitter buffer

    /**
     * @brief Lambda of the Emitter
     * 
     */
    auto emit = [&](Farm::Emitter e) {
        while (true)
        {
            int turn = 0;

            // while the Emitter has something to emit
            while (e.hasNext())
            {
                // get this element
                auto job = e.next();

                // send it to the worker buffer in position #turn
                e2w_buff[turn].send(job);

                //update the #turn
                turn = (++turn) % nw;
            }

            // if the Emitter has finished to emit data, then wait something from the Collector
            auto res = c2e_buff.receive();

            // check #res, then decide either to restart the Emitter or stop the computation
            if (res == Farm::RESTART)
            {
                e.restart();
            }
            else if (res == Farm::EXIT)
            {
                break;
            }
        }

        // send EOS to the Workers
        for (int i = 0; i < nw; i++)
        {
            e2w_buff[i].send(Farm::EOS);
        }
    };

    /**
     * @brief Lamdba of the Worker
     * @param wid the ID of the Worker
     */
    auto work = [&](Farm::Worker w, int wid) {
        while (true)
        {
            // receive something from the Emitter
            auto job = e2w_buff[wid].receive();

            // if #job equals EOS interrupt the Worker and forward the EOS to the Collector
            if (job == Farm::EOS)
            {
                w2c_buff[wid].send(Farm::EOS);
                break;
            }

            // compute the assigned #job
            auto res = w.compute(job);

            // send to the collector
            w2c_buff[wid].send(res);
        }
    };

    /**
     * @brief Lambda of the Collector
     * 
     */
    auto collect = [&](Farm::Collector c) {
        uint turn = 0;
        uint collected = 0;

        while (true)
        {
            // if the buffer in position #turn is empty then skip the current computation
            if (w2c_buff[turn].empty())
            {
                turn = (++turn) % nw;
                continue;
            }

            // receive #res from the buffer in position #turn
            auto res = w2c_buff[turn].receive();

            // if #res = EOS and the Collector has received #nw EOSs then exit
            if (res == Farm::EOS)
            {
                collected++;
                if (collected == nw)
                {
                    break;
                }
            }
            else
            {
                // pass #res to the Collector
                auto val = c.collect(res);

                // if different from CONTINUE means either RESTART or EXIT
                if (val != Farm::CONTINUE)
                {
                    c2e_buff.send(val);
                }
            }
        }
    };

    utimer u(std::to_string(nw) + "," + std::to_string(dim));

    // create one thread for the Emitter
    std::thread emit_thr(emit, emitter);

    // create nw thread for the Workers
    std::vector<std::thread> tids;
    for (int i = 0; i < nw; i++)
    {
        tids.push_back(std::thread(work, worker, i));
    }

    // create one thread for the Collector
    std::thread collect_thr(collect, collector);

    // join all threads
    emit_thr.join();

    for (int i = 0; i < nw; i++)
    {
        tids[i].join();
    }

    collect_thr.join();

    // !! NOTE !!
    //
    // To check that the array is sorted, please remove the comment to the following line.
    //

    // assert(std::is_sorted(vec.begin(), vec.end()));
    return 0;
}