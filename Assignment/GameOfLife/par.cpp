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
    tools::print(table);

    FFarm::Emitter emitter(n, m, nw);
    FFarm::Worker worker(table, res_table);
    FFarm::Collector collector(table, res_table);

    syque<std::pair<int, int>> w_queue;
    syque<int> c_queue;
    syque<bool> f_queue;

    auto emit = [&](FFarm::Emitter e) {
        for (int i = 0; i < niter; i++)
        {
            while (e.hasNext())
            {
                auto job = e.next();
                w_queue.push(job);
            }
            bool res = f_queue.pop();
            if (res == true)
                e.restart();
        }
        for (int i = 0; i < nw; i++)
        {
            w_queue.push(EOS);
        }
    };

    auto work = [&](FFarm::Worker w) {
        while (true)
        {
            auto job = w_queue.pop();
            if (job == EOS)
            {
                c_queue.push(EOC);
                break;
            }
            int res = w.compute(job);
            c_queue.push(res);
        }
    };

    auto collect = [&](FFarm::Collector c) {
        uint64_t nworkers = nw;

        while (true)
        {
            int res = c_queue.pop();
            if (res == EOC && (--nworkers) == 0)
                break;
            bool val = c.collect(res);
            if (val == true)
                f_queue.push(val);
        }
    };

    utimer u("Parallel");

    std::thread emit_thr(emit, emitter);

    std::vector<std::thread> tids;
    for (int i = 0; i < nw; i++)
    {
        tids.push_back(std::thread(work, worker));
    }

    std::thread collect_thr(collect, collector);

    emit_thr.join();

    for (int i = 0; i < nw; i++)
    {
        tids[i].join();
    }

    collect_thr.join();

    tools::print(res_table);

    return 0;
}