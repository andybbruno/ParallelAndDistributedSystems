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

    Table table(n, Row(m, OFF));
    Table res_table(table);
    tools::randomize(table);

    if (doprint)
    {
        tools::print(table);
    }

    FFarm::Emitter emitter(table);
    FFarm::Worker worker;
    FFarm::Collector collector(table, res_table);

    std::vector<buffer<Chunk>> ew_buff(nw); // emitter to worker buffer
    std::vector<buffer<RowID>> wc_buff(nw); // worker to collector buffer
    buffer<bool> ce_buff;                   // collector to emitter buffer

    auto emit = [&](FFarm::Emitter e) {
        for (int i = 0; i < niter; i++)
        {
            int turn = 0;
            while (e.hasNext())
            {
                auto job = e.next();
                ew_buff[turn].send(job);
                turn = (++turn) % nw;
            }
            bool res = ce_buff.receive();
            if (res == true)
                e.restart();
        }
        for (int i = 0; i < nw; i++)
        {
            ew_buff[i].send(EOS);
        }
    };

    auto work = [&](FFarm::Worker w, int wid) {
        while (true)
        {
            auto job = ew_buff[wid].receive();
            if (job == EOS)
            {
                wc_buff[wid].send(EOC);
                break;
            }
            auto res = w.compute(job);
            wc_buff[wid].send(res);
        }
    };

    auto collect = [&](FFarm::Collector c) {
        uint64_t nworkers = nw;
        int turn = 0;
        while (true)
        {
            if (wc_buff[turn].empty())
            {
                turn = (++turn) % nw;
                continue;
            }
            auto res = wc_buff[turn].receive();
            if (res == EOC && (--nworkers) == 0)
            {
                break;
            }
            if (res != EOC)
            {
                bool val = c.collect(res);
                if (val == true)
            {
                ce_buff.send(val);
            }
            }
        }
    };

    utimer u(std::to_string(nw) + "," + std::to_string(n));

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