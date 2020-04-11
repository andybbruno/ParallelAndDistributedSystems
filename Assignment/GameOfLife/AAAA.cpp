#include <iostream>
#include <vector>
#include <thread>
#include "lib/ffarm.cpp"
#include "lib/syque.hpp"
#include "lib/tools.hpp"
#include "lib/utimer.cpp"

int main(int argc, char *argv[])
{
    uint64_t niter = atoi(argv[1]);
    size_t seed = atoi(argv[2]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);
    uint64_t n = atoi(argv[3]);
    uint64_t m = atoi(argv[4]);
    uint64_t nw = atoi(argv[5]);

    std::vector<std::vector<bool>> table(n, std::vector<bool>(m, false));
    std::vector<std::vector<bool>> res_table(table);
    tools::randomize(table);

    FFarm::Emitter emitter(n, m);
    FFarm::Worker worker(table, res_table);
    FFarm::Collector collector(table, res_table);

    syque<std::pair<int, int>> w_queue;
    syque<bool> c_queue;
    bool restart = false;

    auto emit = [&](FFarm::Emitter e) {
        for (int i = 0; i < niter; i++)
        {
            while (e.hasNext())
            {
                auto job = e.next();
                w_queue.push(job);
            }
            e.restart();
        }
        for (int i = 0; i < nw; i++)
        {
            w_queue.push(EOS);
        }
        return;
    };

    auto work = [&](FFarm::Worker w) {
        while (true)
        {
            auto job = w_queue.pop();
            if (job == EOS)
            {
                c_queue.push(false);
                break;
            }
            bool res = w.compute(job);
            c_queue.push(res);
        }
    };

    auto collect = [&](FFarm::Collector c) {
        uint64_t nworkers = nw;

        while (true)
        {
            bool res = c_queue.pop();
            if (res == false && (--nworkers) == 0)
                break;
            c.collect(res);
        }
    };

    utimer u("Parallel");

    // auto emit_thr = new std::thread(emit, emitter);
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

    return 0;
}