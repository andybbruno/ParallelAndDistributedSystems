#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include <algorithm>
#include "core/master_worker.cpp"
#include "core/buffer.cpp"
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
    bool swap = true;
    MaWo::Master master(vec, swap, nw);
    MaWo::Worker worker(vec, swap);

    std::vector<Buffer<MaWo::Task>> m2w_buff(nw); // master to worker buffer

    auto mst = [&](MaWo::Master m) {
        while (swap)
        {
            swap = false;
            int turn = 0;
            while (m.hasNext())
            {
                auto job = m.next();
                m2w_buff[turn].send(job);
                turn = (++turn) % nw;
            }
            m.restart();
            while (m.hasNext())
            {
                auto job = m.next();
                m2w_buff[turn].send(job);
                turn = (++turn) % nw;
            }
        }

        for (int i = 0; i < nw; i++)
        {
            m2w_buff[i].send(MaWo::EOS);
        }
    };

    auto work = [&](MaWo::Worker w, int wid) {
        while (true)
        {
            // std::cout << "WORKER" << std::endl;
            auto job = m2w_buff[wid].receive();
            w.compute(job);
        }
    };

    utimer u(std::to_string(nw) + "," + std::to_string(dim));

    std::thread mst_thr(mst, master);

    std::vector<std::thread> tids;
    for (int i = 0; i < nw; i++)
    {
        tids.push_back(std::thread(work, worker, i));
    }

    mst_thr.join();

    for (int i = 0; i < nw; i++)
    {
        tids[i].join();
    }

    // tools::print(vec);
    assert(std::is_sorted(vec.begin(), vec.end()));
    return 0;
}