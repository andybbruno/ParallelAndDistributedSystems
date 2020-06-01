#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include <algorithm>
#include "core/FarmT.cpp"
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

    std::vector<int> vec = tools::rand_vec(dim, range);
    // tools::print(vec);
    Farm::Emitter emitter(vec, nw);
    Farm::Worker worker(vec);
    Farm::Collector collector(nw);

    std::vector<Buffer<Farm::Task>> e2w_buff(nw); // emitter to worker buffer
    std::vector<Buffer<Farm::Task>> w2c_buff(nw); // worker to collector buffer
    Buffer<Farm::C2E_Flag> c2e_buff;              // collector to emitter buffer

    auto emit = [&](Farm::Emitter e) {
        while (true)
        {
            // std::cout << "EMITTER" << std::endl;

            int turn = 0;
            while (e.hasNext())
            {
                auto job = e.next();
                e2w_buff[turn].send(job);
                turn = (++turn) % nw;
            }
            auto res = c2e_buff.receive();
            if (res == Farm::RESTART)
            {
                e.restart();
            }
            else if (res == Farm::EXIT)
            {
                break;
            }
        }

        for (int i = 0; i < nw; i++)
        {
            e2w_buff[i].send(Farm::EOS);
        }
    };

    auto work = [&](Farm::Worker w, int wid) {
        while (true)
        {
            // std::cout << "WORKER" << std::endl;
            auto job = e2w_buff[wid].receive();
            if (job == Farm::EOS)
            {
                w2c_buff[wid].send(Farm::EOS);
                break;
            }
            auto res = w.compute(job);
            // std::cout << "Wres = " <<  res << std::endl;
            w2c_buff[wid].send(res);
        }
    };

    auto collect = [&](Farm::Collector c) {
        uint turn = 0;
        uint collected = 0;
        while (true)
        {
            // std::cout << "COLLECTOR" << std::endl;
            if (w2c_buff[turn].empty())
            {
                // std::cout << "empty " << nw << std::endl;
                turn = (++turn) % nw;
                continue;
            }

            auto res = w2c_buff[turn].receive();
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
                auto val = c.collect(res);
                if (val != Farm::CONTINUE)
                {
                    c2e_buff.send(val);
                }
            }
        }
    };

    utimer u(std::to_string(nw) + "," + std::to_string(dim));

    std::thread emit_thr(emit, emitter);

    std::vector<std::thread> tids;
    for (int i = 0; i < nw; i++)
    {
        tids.push_back(std::thread(work, worker, i));
    }

    std::thread collect_thr(collect, collector);

    emit_thr.join();

    for (int i = 0; i < nw; i++)
    {
        tids[i].join();
    }

    collect_thr.join();

    // tools::print(vec);
    // assert(std::is_sorted(vec.begin(), vec.end()));
    return 0;
}