#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include <algorithm>
#include "../core/FarmT.cpp"
#include "../lib/buffer.cpp"
#include "../lib/tools.cpp"
#include "../lib/utimer.cpp"

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

    std::vector<long long> emitter_time;
    std::vector<long long> worker_time;
    std::vector<long long> collector_time;

    Farm::Emitter emitter(vec, nw);
    Farm::Worker worker(vec);
    Farm::Collector collector(nw);

    std::vector<Buffer<Farm::Task>> e2w_buff(nw); // emitter to worker buffer
    std::vector<Buffer<Farm::Task>> w2c_buff(nw); // worker to collector buffer
    Buffer<Farm::C2E_Flag> c2e_buff;              // collector to emitter buffer

    auto emit = [&](Farm::Emitter e) {
        while (true)
        {
            auto begin = std::chrono::system_clock::now();

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

            auto end = std::chrono::system_clock::now();
            auto elapsed = end - begin;
            auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            emitter_time.push_back(musec);
        }

        for (int i = 0; i < nw; i++)
        {
            e2w_buff[i].send(Farm::EOS);
        }
    };

    auto work = [&](Farm::Worker w, int wid) {
        while (true)
        {
            auto begin = std::chrono::system_clock::now();

            auto job = e2w_buff[wid].receive();
            if (job == Farm::EOS)
            {
                w2c_buff[wid].send(Farm::EOS);
                break;
            }
            auto res = w.compute(job);
            w2c_buff[wid].send(res);

            auto end = std::chrono::system_clock::now();
            auto elapsed = end - begin;
            auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            worker_time.push_back(musec);
        }
    };

    auto collect = [&](Farm::Collector c) {
        uint turn = 0;
        uint collected = 0;
        while (true)
        {
            auto begin = std::chrono::system_clock::now();
            if (w2c_buff[turn].empty())
            {
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
            auto end = std::chrono::system_clock::now();
            auto elapsed = end - begin;
            auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            collector_time.push_back(musec);
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

    auto avg_emitter = std::accumulate(emitter_time.begin(), emitter_time.end(), 0.0) / emitter_time.size();
    auto avg_worker = std::accumulate(worker_time.begin(), worker_time.end(), 0.0) / worker_time.size();
    auto avg_collector = std::accumulate(collector_time.begin(), collector_time.end(), 0.0) / collector_time.size();

    std::cout << "Emitter:\t" << avg_emitter << std::endl;
    std::cout << "Worker:\t" << avg_worker << std::endl;
    std::cout << "Collector:\t" << avg_collector << std::endl;

    // tools::print(vec);
    // assert(std::is_sorted(vec.begin(), vec.end()));
    return 0;
}