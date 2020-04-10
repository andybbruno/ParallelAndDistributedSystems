#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include "lib/farm.cpp"
#include "lib/tools.hpp"
#include "lib/utimer.cpp"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "Usage is " << argv[0]
                  << " niter seed rows cols nw (debug)" << std::endl;
        return (-1);
    }

    size_t niter = atoi(argv[1]);
    size_t seed = atoi(argv[2]);
    (seed == 0) ? srand(time(NULL)) : srand(seed);
    size_t n = atoi(argv[3]);
    size_t m = atoi(argv[4]);
    size_t nw = atoi(argv[5]);
    bool debug = (argc != 6);

    std::mutex task_mutex;
    std::mutex res_mutex;

    std::queue<Cell> t_queue;
    std::queue<Cell> r_queue;

    std::vector<std::vector<bool>> table(n, std::vector<bool>(m, false));
    std::vector<std::vector<bool>> res_table(n, std::vector<bool>(m, false));
    tools::randomize(table);

    // kind of three concurrent activities
    // place input tasks into the input queue
    auto emit = [&](Farm::Emitter s) {
        while (s.hasNext())
        {
            auto t = s.next();
            task_mutex.lock();
            t_queue.push(t);
            task_mutex.unlock();
            if (debug)
                std::cout << "Emitted " << t << std::endl;
        }
        // for as many workers to terminate, push END
        for (int i = 0; i < nw; i++)
        {
            task_mutex.lock();
            t_queue.push(END);
            task_mutex.unlock();
        }
        if (debug)
            std::cout << "Emitted " << nw << " ENDs" << std::endl;
        return;
    };

    // processing tasks to results in parallel
    auto work = [&](Farm::Worker w, uint8_t wn) {
        while (true)
        {
            task_mutex.lock();
            if (!t_queue.empty())
            {
                auto t = t_queue.front();
                t_queue.pop();
                task_mutex.unlock();
                if (debug)
                    std::cout << "W" << wn << " got " << t << std::endl;
                if (t == END)
                    break;
                auto r = w.compute(t);
                if (debug)
                    std::cout << "W" << wn << " computed "
                              << r << " out of " << t << std::endl;
                res_mutex.lock();
                r_queue.push(r);
                res_mutex.unlock();
            }
            else
            {
                task_mutex.unlock();
            }
        }
        if (debug)
            std::cout << "W" << wn << " got END " << std::endl;
        res_mutex.lock();
        r_queue.push(END);
        res_mutex.unlock();
        return;
    };

    // process results
    auto collect = [&](Farm::Collector d, int nw) {
        while (true)
        {
            res_mutex.lock();
            if (!r_queue.empty())
            {
                auto t = r_queue.front();
                r_queue.pop();
                res_mutex.unlock();
                if (debug)
                    std::cout << "Drain got " << t << std::endl;
                if (t == END && (--nw) == 0)
                    break;
                d.process(t);
                if (debug)
                    std::cout << "Drain processed " << t << std::endl;
            }
            else
            { // else re-try (after a while?)
                res_mutex.unlock();
            }
        }
        d.decrement();
        table = std::move(res_table);
        res_table = std::vector<std::vector<bool>>(n, std::vector<bool>(m, false));
    };

    Farm::Emitter emitter(n, m, niter);
    Farm::Worker worker(table);
    Farm::Collector collector(res_table, niter);

    auto t0 = std::chrono::system_clock::now();

    auto e_thr = new std::thread(emit, emitter);
    // if (debug)
    //     std::cout << "Started source" << std::endl;

    auto c_thr = new std::thread(collect, collector, nw);
    // if (debug)
    //     std::cout << "Started Drain" << std::endl;

    std::vector<std::thread *> tids(nw);
    for (int i = 0; i < nw; i++)
    {
        // if (debug)
        //     std::cout << "Going to start worker " << i << std::endl;
        tids[i] = new std::thread(work, worker, i);
    }

    // now await termination
    if (debug)
        std::cout << "Awaiting Source " << std::endl;
    e_thr->join();
    for (int i = 0; i < nw; i++)
    {
        if (debug)
            std::cout << "Awaiting worker " << i << std::endl;
        tids[i]->join();
    }
    if (debug)
        std::cout << "Awaiting Drain" << std::endl;
    c_thr->join();

    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - t0).count();

    std::cout << std::endl
              << "Parallel execution with " << nw << " threads took " << elapsed << " msecs" << std::endl
              << std::endl;
    return 0;
}