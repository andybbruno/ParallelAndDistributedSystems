#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include <algorithm>
#include "../core/FarmV.cpp"
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

    std::vector<long long> e2w_time;
    std::vector<long long> w2c_time;
    std::vector<long long> c2e_time;

    Farm::Emitter emitter(vec, nw);
    Farm::Worker worker;
    Farm::Collector collector(vec, nw);

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

                auto begin = std::chrono::system_clock::now();
                e2w_buff[turn].send(job);
                auto end = std::chrono::system_clock::now();
                auto elapsed = end - begin;
                auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                e2w_time.push_back(musec);

                turn = (++turn) % nw;
            }

            auto begin = std::chrono::system_clock::now();
            auto res = c2e_buff.receive();
            auto end = std::chrono::system_clock::now();
            auto elapsed = end - begin;
            auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            c2e_time.push_back(musec);

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
            auto begin = std::chrono::system_clock::now();
            e2w_buff[i].send(Farm::EOS);
            auto end = std::chrono::system_clock::now();
            auto elapsed = end - begin;
            auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            e2w_time.push_back(musec);
        }
    };

    auto work = [&](Farm::Worker w, int wid) {
        while (true)
        {
            auto begin = std::chrono::system_clock::now();
            auto job = e2w_buff[wid].receive();
            auto end = std::chrono::system_clock::now();
            auto elapsed = end - begin;
            auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            e2w_time.push_back(musec);

            if (job == Farm::EOS)
            {
                auto begin = std::chrono::system_clock::now();
                w2c_buff[wid].send(Farm::EOS);
                auto end = std::chrono::system_clock::now();
                auto elapsed = end - begin;
                auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                w2c_time.push_back(musec);

                break;
            }
            auto res = w.compute(job);

            begin = std::chrono::system_clock::now();
            w2c_buff[wid].send(res);
            end = std::chrono::system_clock::now();
            elapsed = end - begin;
            musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            w2c_time.push_back(musec);
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

            auto begin = std::chrono::system_clock::now();
            auto res = w2c_buff[turn].receive();
            auto end = std::chrono::system_clock::now();
            auto elapsed = end - begin;
            auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            w2c_time.push_back(musec);

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
                    auto begin = std::chrono::system_clock::now();
                    c2e_buff.send(val);
                    auto end = std::chrono::system_clock::now();
                    auto elapsed = end - begin;
                    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                    c2e_time.push_back(musec);
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

    auto avg_e2w = std::accumulate(e2w_time.begin(), e2w_time.end(), 0.0) / e2w_time.size();
    auto avg_w2c = std::accumulate(w2c_time.begin(), w2c_time.end(), 0.0) / w2c_time.size();
    auto avg_c2e = std::accumulate(c2e_time.begin(), c2e_time.end(), 0.0) / c2e_time.size();

    std::cout << "Emitter 2 Worker:\t" << avg_e2w << std::endl;
    std::cout << "Worker 2 Collector:\t" << avg_w2c << std::endl;
    std::cout << "Collector 2 Emitter:\t" << avg_c2e << std::endl;

    // tools::print(vec);
    // assert(std::is_sorted(vec.begin(), vec.end()));
    return 0;
}