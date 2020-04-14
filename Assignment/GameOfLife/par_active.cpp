#include <iostream>
#include <vector>
#include <thread>
#include "lib/ffarm.cpp"
#include "lib/buffer.cpp"
#include "lib/tools.hpp"
#include "lib/utimer.cpp"

int main(int argc, char *argv[])
{
    int niter = atoi(argv[1]);
    int seed = atoi(argv[2]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);
    int n = atoi(argv[3]);
    int m = atoi(argv[4]);
    int nw = atoi(argv[5]);
    bool doprint = (argc > 6) ? true : false;

    std::vector<std::vector<bool>> table(n, std::vector<bool>(m, false));
    std::vector<std::vector<bool>> res_table(table);
    tools::randomize(table);

    if (doprint)
    {
        tools::print(table);
    }

    FFarm::Emitter emitter(n, m, nw);
    FFarm::Worker worker(table, res_table);
    FFarm::Collector collector(table, res_table);

    std::vector<buffer<std::pair<int, int>>> w_buff(nw);
    std::vector<buffer<int>> c_buff(nw);
    buffer<bool> f_buff;

    auto emit = [&](FFarm::Emitter e) {
        for (int i = 0; i < niter; i++)
        {
            int turn = 0;
            while (e.hasNext())
            {
                auto job = e.next();
                w_buff[turn].send(job);
                turn = (++turn) % nw;
            }
            bool res = f_buff.receive();
            if (res == true)
                e.restart();
        }
        for (int i = 0; i < nw; i++)
        {
            w_buff[i].send(EOS);
        }
    };

    auto work = [&](FFarm::Worker w, int wid) {
        while (true)
        {
            auto job = w_buff[wid].receive();
            if (job == EOS)
            {
                c_buff[wid].send(EOC);
                break;
            }
            int res = w.compute(job);
            c_buff[wid].send(res);
        }
    };

    auto collect = [&](FFarm::Collector c) {
        uint64_t nworkers = nw;
        int turn = 0;
        while (true)
        {
            if (c_buff[turn].empty())
            {
                turn = (++turn) % nw;
                continue;
            }
            int res = c_buff[turn].receive();
            if (res == EOC && (--nworkers) == 0)
                break;
            bool val = c.collect(res);
            if (val == true)
                f_buff.send(val);
        }
    };

    utimer u("Parallel");

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

    if (doprint)
    {
        tools::print(table);
    }

    return 0;
}