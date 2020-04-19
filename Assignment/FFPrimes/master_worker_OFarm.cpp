#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <cstdlib>
#include <ff/utils.hpp>
#include <ff/parallel_for.hpp>

using namespace ff;
using ull = unsigned long long;
using Range = std::pair<ull, ull>;

struct Task
{
    ull start;
    ull end;
    std::vector<ull> primes;

    Task(ull s, ull e) : start(s), end(e)
    {
        // primes.reserve(end - start);
    }
};

static inline bool is_prime(ull n)
{
    if (n <= 3)
        return n > 1; // 1 is not prime !

    if (n % 2 == 0 || n % 3 == 0)
        return false;

    for (ull i = 5; i * i <= n; i += 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

struct Master : ff_node_t<Task>
{
    ull start = 0;
    ull end = 0;
    ull nw = 0;
    bool print = false;

    std::vector<std::vector<ull> *> res;
    size_t ntasks = 0;

    Master(ull start, ull end, size_t nw, bool print) : start(start), end(end), nw(nw), print(print) {}
    Task *svc(Task *task)
    {
        if (task == nullptr)
        {
            auto dim = (end - start) / nw;
            auto mod = (end - start) % nw;
            auto a = start;
            auto b = start + dim + mod;

            for (size_t i = 0; i < nw; ++i)
            {
                ff_send_out(new Task(a, b));
                a = b + 1;
                b += dim;
            }
            return GO_ON;
        }

        Task &t = *task;

        if (print)
            std::cout << "Master received " << t.start << " - " << t.end << "\n";

        res.push_back(&t.primes);

        // delete task;
        if (++ntasks == nw)
            return EOS;
        return GO_ON;
    }
    void svc_end()
    {
        ull npr = 0;
        for (auto p : res)
        {
            npr += p->size();
        }
        std::cout << "Found " << npr << " primes\n";
        if (print)
        {
            for (auto x : res)
            {
                std::cout << x << " ";
            }
            std::cout << "\n\n";
        }
    }
};

struct Worker : ff_node_t<Task>
{
    Task *svc(Task *task)
    {
        Task &t = *task;
        for (auto i = t.start; i <= t.end; i++)
        {
            if (is_prime(i))
                t.primes.push_back(i);
        }
        return task;
    }
};

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "use: " << argv[0] << " start end nw [print=off|on]\n";
        return -1;
    }
    ull start = atoll(argv[1]);
    ull end = atoll(argv[2]);
    int nw = atoi(argv[3]);
    bool doprint = (argc > 4) ? true : false;

    Master master(start, end, nw, doprint);

    std::vector<std::unique_ptr<ff_node>> W;
    for (size_t i = 0; i < nw; ++i)
    {
        W.push_back(make_unique<Worker>());
    }

    ff_OFarm<Task> farm(std::move(W));
    farm.add_emitter(master);
    // farm.remove_collector();
    farm.wrap_around();

    ffTime(START_TIME);
    if (farm.run_and_wait_end() < 0)
    {
        error("running farm");
        return -1;
    }
    ffTime(STOP_TIME);
    std::cout << "Time: " << ffTime(GET_TIME) << " (ms)\n";

    return 0;
}